// includes & usings {{{
#include "block_node.hh"

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
BlockNode::BlockNode (ClientHandler* net) : Node () { 
  network = net;
  network_size = context.settings.get<vec_str>("network.nodes").size();
}

BlockNode::~BlockNode() { }
// }}}
// replicate_message {{{
//! @brief Compute the right and left node of the current node
//! and send its replicas of the given block
void BlockNode::replicate_message(IOoperation* m) {
  vector<int> nodes;
  for (int i=1; i < 3; i++) {
    if(i%2 == 1) {
      nodes.push_back ((id + (i+1)/2 + network_size) % network_size);
    } else {
      nodes.push_back ((id - i/2 + network_size) % network_size);
    }
  }
  network->send_and_replicate(nodes, m);
}
// }}}
// block_insert_local {{{
//! @brief This method insert the block locally and replicated it.
bool BlockNode::block_insert_local(Block& block, bool replicate) {
  local_io.write(block.first, block.second);

  if (replicate) {
    INFO("[DFS] Saving locally BLOCK: %s", block.first.c_str());
    IOoperation io;
    io.operation = messages::IOoperation::OpType::BLOCK_INSERT_REPLICA;
    io.block = move(block);
    replicate_message(&io);
  } else {
    INFO("[DFS] Saving replica locally BLOCK: %s", block.first.c_str());
  }

  return true;
}
// }}}
// block_read_local {{{
//! @brief This method read the block locally.
bool BlockNode::block_read_local(Block& block, uint64_t off, uint64_t len, bool ignore_params) {
  INFO("BLOCK REQUEST: %s", block.first.c_str());
  block.second = local_io.read(block.first, off, len, ignore_params);
  return true;
}
// }}}
// block_delete_local {{{
//! @brief This method read the block locally.
bool BlockNode::block_delete_local(Block& block, bool replicate) {
  local_io.remove(block.first);

  INFO("[DFS] Removed locally BLOCK: %s", block.first.c_str());

  if (replicate) {
    IOoperation io;
    io.operation = messages::IOoperation::OpType::BLOCK_DELETE_REPLICA;
    io.block = move(block);
    replicate_message(&io);
  }

  return true;
}
// }}}
// block_update_local {{{
bool BlockNode::block_update_local(Block& block, uint32_t pos, uint32_t len, bool replicate) {
  local_io.update(block.first, block.second, pos, len);

  if (replicate) {
    INFO("Block %s updated real host", block.first.c_str());
    IOoperation io;
    io.operation = messages::IOoperation::OpType::BLOCK_UPDATE_REPLICA;
    io.pos = pos;
    io.length = len;
    io.block = move(block);
    replicate_message(&io);

  } else {
    INFO("Block replica %s updated real host", block.first.c_str());
  }
  return true;
}
// }}}
}
