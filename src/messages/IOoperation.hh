#pragma once
#include "message.hh"
#include "blockinfo.hh"
#include "../common/block.hh"
#include "../common/blockmetadata.hh"

namespace eclipse {
namespace messages {

struct IOoperation: public Message {
  std::string get_type() const override;

  enum class OpType {
    BLOCK_INSERT,
    BLOCK_INSERT_REPLICA,
    BLOCK_DELETE,
    BLOCK_DELETE_REPLICA,
    BLOCK_REQUEST,
    BLOCK_TRANSFER,
    BLOCK_UPDATE,
    BLOCK_UPDATE_REPLICA,
	LOGICAL_BLOCK_REQUEST,
	LBLOCK_MANAGER_INIT,
	LBLOCK_MANAGER_READ,
	LBLOCK_STOP,
	LBLOCK_MANAGER_DESTROY
  };

  OpType operation;
  std::string option;

  uint64_t pos = 0;
  uint64_t length = 0;
  BlockMetadata block_metadata;
  Block block;
};

}
}
