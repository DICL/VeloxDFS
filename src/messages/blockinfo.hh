#pragma once
#include "message.hh"
#include <cstdint>
#include <string>
#include <vector>

namespace eclipse {
  namespace messages {
    struct BlockInfo: public Message {
      BlockInfo();
      BlockInfo(std::string, uint32_t, std::string, uint64_t, uint64_t, int, int, int, std::string&);
      BlockInfo(std::string, uint32_t, std::string, uint64_t, uint64_t, int, int);

      std::string get_type() const override;

      std::string name;
      uint32_t hash_key;
      std::string file_name;
      uint64_t seq;
      uint64_t size;
      int net_id;
      int replica;
      int check_commit;
      std::string content;
    };
  }
}
