#pragma once
#include <string>

// role: stores block contexts as a string

namespace eclipse {
  struct Block {
    friend class FileInfo;
    private:
      std::string block;
  }
}
