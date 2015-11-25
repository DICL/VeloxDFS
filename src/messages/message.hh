//
// @rationale Every message class should inherite from this file
//            the requirements are:
//             - All the serializable members should be public
//             - Mandatory overloading of virtual std::string get_type()
//             - Non-arguments constructor
//
#pragma once

#include <string>

namespace messages {
  struct Message {
    Message() = default;
    Message(std::string, std::string);
    virtual ~Message () {}

    virtual std::string get_type() const;

    std::string get_origin() const;
    std::string get_destination() const;

    Message& set_origin(std::string);
    Message& set_destination(std::string);

    std::string origin, destination;
  };
}
