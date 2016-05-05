#pragma once
#include "message.hh"
#include <functional>

namespace eclipse {
namespace messages {

using message_fun = std::function<void(Message*)>;

template <typename T>
struct Executable: public Message {
  virtual void exec(T*, message_fun) = 0;
  virtual std::string get_type() const = 0;
};

}
}
