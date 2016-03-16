#pragma once
#include "message.hh"

namespace eclipse {
namespace messages {

struct IDataInfo: public Message {
  IDataInfo() = default;
  ~IDataInfo() = default;

  std::string get_type() const override;

  uint32_t job_id;
  uint32_t map_id;
  uint32_t num_reducer;
};

}  // namespace messages
}  // namespace eclipse
