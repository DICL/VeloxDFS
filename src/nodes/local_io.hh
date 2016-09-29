#pragma once
#include <string>
#include <cstdint>

namespace eclipse {

  /**
   * Represents the local disk. 
   * Local_io automatically computes the prefix of the path.
   */
  class Local_io {
    public:
      Local_io();
      void write(std::string, std::string&);
      void update(std::string, std::string, uint64_t, uint64_t);
      std::string read(std::string);
      std::string read_metadata();
      std::string pread(std::string, uint64_t, uint64_t);
      void remove(std::string);
      bool format();
  };

}
