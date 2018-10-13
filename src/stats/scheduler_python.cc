#include "logical_blocks_scheduler.hh"
#include "../common/context_singleton.hh"
#include "../messages/blockinfo.hh"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#define WRITE 1
#define READ  0
#define OUTPUT_LEN (1 << 18)

using namespace eclipse::logical_blocks_schedulers;
using namespace eclipse;
using namespace eclipse::messages;
using namespace boost::property_tree;
using namespace std;

string input = " \
{\
  \"nodes\" : [0, 1, 2, 3, 4, 5],\
  \"io\"    : [0.1, 0.4, 0.2, 0.5, 0.6, 0.3],\
  \"chunksPerBlock\" : 10,\
  \"chunks\" : {\
    \"0\" : [0, 1, 6, 7, 5, 12, 13, 11, 18, 19, 17, 24, 25, 23, 30, 31, 29, 36, 37, 35, 42, 43, 41, 48, 49, 47, 54, 55, 53, 60, 61, 59, 66, 67, 65, 72, 73, 71, 78, 79, 77, 84, 85, 83, 90, 91, 89, 96, 97, 95],\
    \"1\" : [1, 2, 0, 7, 8, 6, 13, 14, 12, 19, 20, 18, 25, 26, 24, 31, 32, 30, 37, 38, 36, 43, 44, 42, 49, 50, 48, 55, 56, 54, 61, 62, 60, 67, 68, 66, 73, 74, 72, 79, 80, 78, 85, 86, 84, 91, 92, 90, 97, 98, 96],\
    \"2\" : [2, 3, 1, 8, 9, 7, 14, 15, 13, 20, 21, 19, 26, 27, 25, 32, 33, 31, 38, 39, 37, 44, 45, 43, 50, 51, 49, 56, 57, 55, 62, 63, 61, 68, 69, 67, 74, 75, 73, 80, 81, 79, 86, 87, 85, 92, 93, 91, 98, 99, 97],\
    \"3\" : [3, 4, 2, 9, 10, 8, 15, 16, 14, 21, 22, 20, 27, 28, 26, 33, 34, 32, 39, 40, 38, 45, 46, 44, 51, 52, 50, 57, 58, 56, 63, 64, 62, 69, 70, 68, 75, 76, 74, 81, 82, 80, 87, 88, 86, 93, 94, 92, 99, 0, 98],\
    \"4\" : [4, 5, 3, 10, 11, 9, 16, 17, 15, 22, 23, 21, 28, 29, 27, 34, 35, 33, 40, 41, 39, 46, 47, 45, 52, 53, 51, 58, 59, 57, 64, 65, 63, 70, 71, 69, 76, 77, 75, 82, 83, 81, 88, 89, 87, 94, 95, 93, 99],\
    \"5\" : [5, 6, 4, 11, 12, 10, 17, 18, 16, 23, 24, 22, 29, 30, 28, 35, 36, 34, 41, 42, 40, 47, 48, 46, 53, 54, 52, 59, 60, 58, 65, 66, 64, 71, 72, 70, 77, 78, 76, 83, 84, 82, 89, 90, 88, 95, 96, 94]\
  }}";


void addblock(FileDescription& file_desc, std::vector<std::string> nodes, std::vector<int> sblock, 
    string host, int block_seq, Histogram*);

void scheduler_python::generate(FileDescription& file_desc, std::vector<std::string> nodes) {
  // PREPARE INPUT
  string args, output;
  //pid_t wpid;
  int inputfd[2], outputfd[2]; //, status = 0;

  std::stringstream ss_input;
  // Serialize input
  {
    ptree pt, nodes_child, io_child, chunks_child;
    auto io_vec = listener->get_io_stats();
    INFO("GOT IO STATS");

    for (auto& node: nodes) {
      int node_id = find(nodes.begin(), nodes.end(), node) - nodes.begin();
      ptree node_child;
      node_child.put("", node_id);
      nodes_child.push_back({"", node_child});
    }

    for (auto& io: io_vec) {
      ptree node_child;
      node_child.put("", to_string(io.first));
      io_child.push_back({"", node_child});
    }

    pt.put("chunksPerBlock", 10);
    pt.add_child("nodes", nodes_child);
    pt.add_child("io", io_child);

    std::map<std::string, ptree> ptree_map;
    for (int i = 0; i < (int)file_desc.blocks.size(); i++) {
      int id = find(nodes.begin(), nodes.end(), file_desc.block_hosts[i]) - nodes.begin();
      string key = "chunks." + to_string(id);

      ptree chunks_value;
      chunks_value.put("", i);

      auto it = ptree_map.find(key);
      if (it != ptree_map.end()) {
        it->second.push_back({"", chunks_value});

      } else {
        ptree tmp;
        tmp.push_back({"", chunks_value});
        ptree_map.insert({key, tmp});
      }
    }

    for (auto& it : ptree_map) {
      pt.add_child(it.first, it.second);
    }

    json_parser::write_json(ss_input, pt);
    printf("%s", ss_input.str().c_str());
  }

  pipe(inputfd);
  pipe(outputfd);

  // LAUNCH COMMAND
  pid_t pid;
  if ((pid = fork()) == -1) {
    perror("FORK");
    exit (-1);
  }

  if (pid == 0) {
    dup2(inputfd[READ], STDIN_FILENO);
    dup2(outputfd[WRITE], STDOUT_FILENO);

    close(inputfd[WRITE]);
    close(outputfd[READ]);
    execl(PY_SCHEDULER_SCRIPT, PY_SCHEDULER_SCRIPT, NULL); 
    
  } else {

    // I am the main process
    write(inputfd[WRITE], ss_input.str().c_str(), ss_input.str().length());
    //printf("%s", input.c_str());
    //write(inputfd[WRITE], input.c_str(), input.length());
    close(inputfd[READ]);
    close(inputfd[WRITE]);

    output.reserve(OUTPUT_LEN);
    //while ((wpid = wait(&status)) > 0); 

    while(!read(outputfd[READ], &output[0], OUTPUT_LEN));
    close(outputfd[WRITE]);
    close(outputfd[READ]);

    printf("FIRST: %s", output.c_str());
    std::stringstream ss;
    ss << output.c_str();
    INFO("BEFORE PARSING");

    {
      ptree pt;
      json_parser::read_json(ss, pt);

      // Per each node
      int seq = 0;
      file_desc.n_lblock = 0;
      for (auto& child_pt : pt) {
        int host_id = atoi(child_pt.first.c_str());
        auto& sblocks = child_pt.second;

        // Per each logical block
        for (auto& sblock : sblocks) {
          std::vector<int> sblock_v;

          // Per each chunk
          for (auto& chunks : sblock.second) {
            sblock_v.push_back(chunks.second.get_value<int>());
          }
          addblock(file_desc, nodes, sblock_v, nodes[host_id], seq++, boundaries);
          file_desc.n_lblock++;

        }
      }

    }
  }
}

void addblock(FileDescription& file_desc, std::vector<std::string> nodes, std::vector<int> sblock, 
    string host, int block_seq, Histogram* boundaries) {

  uint32_t which_node = find(nodes.begin(), nodes.end(), host) - nodes.begin();
  logical_block_metadata metadata;
  metadata.seq = block_seq;
  metadata.name = string("logical_") + file_desc.name + "_" + to_string(block_seq);
  metadata.file_name = file_desc.name;
  metadata.hash_key = boundaries->random_within_boundaries(which_node);
  metadata.host_name = host;

  for (int block_id : sblock) {
    BlockInfo physical_block;
    physical_block.name      = file_desc.blocks[block_id];
    physical_block.file_name = file_desc.name;
    physical_block.hash_key  = file_desc.hash_keys[block_id];
    physical_block.size      = file_desc.block_size[block_id];
    physical_block.node      = host;

    metadata.size += physical_block.size; 

    metadata.physical_blocks.push_back(physical_block);
  }

  file_desc.logical_blocks.push_back(metadata);
}
