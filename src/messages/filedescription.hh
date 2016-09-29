#pragma once
#include "message.hh"
#include <vector>

namespace eclipse {
  namespace messages {
    struct FileDescription: public Message {
      std::string get_type() const override;
      std::string name;
      uint64_t size;
      std::vector<std::string> block_name;
      std::vector<uint64_t> block_size;
      std::vector<int> net_id;
      int replica;
    };
  }
}

