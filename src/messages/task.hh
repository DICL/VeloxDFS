#pragma once

#include "message.hh"
#include "mapreduce/task.hh"

namespace eclipse {
namespace messages {

struct Task: public Message {
  Task() = default;
  ~Task() = default;

  std::string get_type() const override;

  Task& set_id(int);
  Task& set_type(int);
  Task& set_input_path(std::string);

  int get_id();
  int get_type();
  std::string get_input_path();

  int id, type;
  std::string library, func_name, input_path;
};

eclipse::messages::Task* serialize (eclipse::Task*);
eclipse::Task* deserialize (Task*);

}
}

