#pragma once

#include "message.hh"
#include <string>
#include <vector>
#include <iostream>
#include "fileinfo.hh"

namespace eclipse {
namespace messages {

struct FileList: public Message {
  FileList() = default;
  FileList(std::vector<FileInfo>);

  std::vector<FileInfo> data;
  std::string get_type() const override;
};

}
}
