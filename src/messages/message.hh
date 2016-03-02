//
// @rationale Every message class should inherite from this file
//            the requirements are:
//             - All the serializable members should be public
//             - Mandatory overloading of virtual std::string get_type()
//             - Non-arguments constructor
//
#pragma once

#include <string>

namespace eclipse {
namespace messages {

struct Message {
  Message() = default;
  Message(int, int);
  virtual ~Message () {}

  virtual std::string get_type() const = 0;

  int get_origin() const;
  int get_destination() const;

  Message& set_origin(int);
  Message& set_destination(int);

  int origin, destination;
};

}
}
