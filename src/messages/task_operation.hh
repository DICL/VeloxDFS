#pragma once
#include "message.hh"
//#include "blockinfo.hh"
//#include "../common/block.hh"
//#include "../common/blockmetadata.hh"
#include "../common/logical_block_metadata.hh"

namespace eclipse {
namespace messages {

struct TaskOperation : public Message {
  std::string get_type() const override;

  enum class OpType {
	TASK_INIT,
	TASK_DESTROY
  };

  OpType operation;
  std::string job_id;
  int tmg_id = 0;
  //int replica_chunk_num[2];
  std::string file;

  struct logical_block_metadata lblock_metadata;

};

}
}
