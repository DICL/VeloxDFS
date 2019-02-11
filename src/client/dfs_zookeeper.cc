#include "dfs.hh"
#include "model/metadata.hh"
#include "../common/context_singleton.hh"
#include <zookeeper/zookeeper.h>
#include <string>
#include <utility>
#include <mutex>

using namespace std;
using namespace velox::model;

namespace velox {
namespace {

const uint64_t BUFFER_SIZE = (1 << 24); // 8 MiB

void connect_to_zk(zhandle_t* zk) {
  string addr = GET_STR("addons.zk.addr");
  int port = GET_INT("addons.zk.port");
  string addr_port = addr + ":" + to_string(port);

  zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
  zk = zookeeper_init(addr_port.c_str(), NULL, 10000, 0, 0, 0);
}


int next_physical_block(metadata& md, uint32_t prefix, std::string block_name, block_metadata& block) {
  static zhandle_t* zk = nullptr;
  static int currentchunk = 0;

  mutex mux;
  std::lock_guard<mutex> lock (mux);

  block_metadata logical_block;

  // initialize just once
  if (!zk) {
    auto it = find_if(md.block_data.begin(), md.block_data.end(), [block_name] (auto& elem) { 
            return elem.name == block_name; 
          });

    logical_block = *it;

    connect_to_zk(zk);
  }

  const string zk_prefix = to_string(prefix);
  int currentSplitNumChunks = md.num_static_blocks;
  block_metadata* chunk;

  // Try to create a node Atomic operation
  while (currentchunk < currentSplitNumChunks) {

    chunk = &logical_block.chunks.at(currentchunk);

    // Dynamic chunk selection
    if (chunk->index >= (int)md.num_static_blocks -1) {

      string chunkPath = zk_prefix + to_string(chunk->index);

      int rc = zoo_create(zk, chunkPath.c_str(), "processing", strlen("processing"), &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);

      // Already exists
      if (rc == ZNODEEXISTS) {
        currentchunk++;
        continue;

        // Retry on disconnect
      } else if (rc == ZINVALIDSTATE) {
        connect_to_zk(zk);
        continue; 
        INFO("Reconnect to zk");

        // Very bad error
      } else if (rc != ZOK) {
        zookeeper_close(zk);
        ERROR("Messed up with the conf to connect to zk");
        return -1;
      }
    }

    // If we found a availible chunk
    INFO("I got a new chunk: %s realindex: %s",  currentchunk, chunk->index);
    currentchunk++;
    break;
  }

  // Reached EOF
  if (currentchunk == currentSplitNumChunks) {
    zookeeper_close(zk);
    return -1;
  }

  block = *chunk;
  return currentchunk;
}

int read_logical_block(metadata& md, uint32_t prefix, string block_name, uint64_t pos, char* buf, uint64_t off, uint64_t len) {
  static vector<block_metadata> current_blocks;
  static uint64_t size = 0; 

  int i = 0; 
  uint64_t totalSize = 0;

  // Get new chunk if no bytes to read
  if (pos >= size) {
    block_metadata next_block;
    next_physical_block(md, prefix, block_name, next_block);
    size +=  next_block.size;
    current_blocks.push_back(next_block);
  }

  // Find chunk to read
  for (block_metadata chunk : current_blocks) {
    if (chunk.size + totalSize > pos) {
      break;
    }
    totalSize += chunk.size;
    i++;
  }

  if (i == (int)current_blocks.size()) {
    return -1;
  }

  block_metadata theChunk = current_blocks.at(i);
  long chunkOffset = pos - totalSize;

  int lenToRead = (int)std::min(len, theChunk.size - chunkOffset);

  long readBytes = velox::read_chunk(theChunk.file_name, theChunk.host, buf, off, 
      chunkOffset, lenToRead);

  return (int)readBytes;
}

}

int lean_peek(velox::model::metadata& md, uint32_t prefix, string block_name) {
  static uint64_t bufferOffset = 0;
  static uint64_t remainingBytes = 0;
  static uint64_t pos = 0;
  static char buffer[BUFFER_SIZE];

  bufferOffset %= BUFFER_SIZE;
  if (bufferOffset == 0 || remainingBytes == 0) {
    bufferOffset = 0;
    remainingBytes = read_logical_block(md, prefix, block_name, pos, buffer, bufferOffset, BUFFER_SIZE);
  }

  if (remainingBytes <= 0) {
    return -1;
  }

  int ret = buffer[bufferOffset];

  // Increment/decrement counters
  pos++;
  remainingBytes--;
  bufferOffset++;

  return ret;

}
}
