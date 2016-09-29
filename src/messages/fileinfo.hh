#pragma once
#include "message.hh"

namespace eclipse {
  namespace messages {
    struct FileInfo: public Message {
      FileInfo();
      FileInfo(std::string, uint32_t, uint64_t, uint64_t, std::string, int);
      FileInfo& operator=(const FileInfo&);
      std::string get_type() const override;

      std::string name;
      uint32_t hash_key;
      uint64_t size;
      uint64_t num_block;
      std::string type;
      int replica;
    };
  }
}
