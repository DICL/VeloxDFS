#pragma once

namespace eclipse{

struct blob: public Message {
  std::string get_type() const override;
  std::string name;
  std::string content;
  uint32_t size;
};
  
}
