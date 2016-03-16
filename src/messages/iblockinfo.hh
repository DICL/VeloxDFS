#pragma once
#include "message.hh"

namespace eclipse {
namespace messages {

struct IBlockInfo: public Message {
  IBlockInfo() = default;
  ~IBlockInfo() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t reducer_id;
  uint32_t block_seq;
};

}
}
