#pragma once
#include "message.hh"
#include <cstdint>

namespace eclipse {
  namespace messages {
    struct NodeInfo: public Message {
      NodeInfo();
      NodeInfo(std::string, std::string, int, int);
      NodeInfo& operator=(const NodeInfo&);
      std::string get_type() const override;

      std::string name;
      std::string file_name;
      int id;
      int net_id;
    };
  }
}
