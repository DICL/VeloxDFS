#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
namespace messages {

  struct BlockInfo: public Message {
	  std::string get_type() const override;

	  std::string name;
	  std::string primary_file;
	  std::string file_name;
	  unsigned int seq;
	  uint32_t hash_key;
	  uint64_t size;
	  unsigned int type;
	  int replica;
	  //uint32_t chunk_seq;
	  uint32_t primary_seq;
	  uint64_t offset;
	  uint64_t foffset;


	  std::string node;
	  std::string l_node;
	  std::string r_node;
	  unsigned int is_committed;
	  std::string content;
  };
}
}
