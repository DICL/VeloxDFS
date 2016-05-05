#pragma once
#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "file.hh"


namespace eclipse {
namespace messages {

struct List_files: public Message {
  List_files() = default;
  List_files(std::vector<File>);

  std::string get_type() const override;
  std::vector<File> data;
};

}
}
