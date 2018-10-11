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
    void write(const std::string&, const std::string&);
    void update(const std::string&, const std::string&, uint32_t, uint32_t);
    std::string read(const std::string&);
    std::string read(const std::string&, uint32_t, uint32_t);
    std::string read(const std::string&, uint32_t, uint32_t, bool);
    std::string read_metadata();
    void remove(const std::string&);
    bool format();

    Local_io();
   
  private:
    std::string disk_path;
};

}
