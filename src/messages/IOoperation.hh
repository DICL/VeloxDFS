#pragma once
#include "message.hh"
#include "../common/block.hh"
#include "../common/blockmetadata.hh"

namespace eclipse {
namespace messages {

struct IOoperation: public Message {
  std::string get_type() const override;

  std::string operation;
  std::string option;

  uint32_t pos = 0;
  uint32_t length = 0;

  BlockMetadata block_metadata;
  Block block;
};

}
}
