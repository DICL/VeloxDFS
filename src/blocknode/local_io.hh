#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <thread>
#include <fstream>
#include "../messages/blockinfo.hh"
#include <memory>

namespace eclipse {

/**
 * Represents the local disk. 
 * Local_io automatically computes the prefix of the path.
 */
class Local_io {
  public:
    void write(const std::string&, const std::string&);
    //void update(const std::string&, const std::string&, uint32_t, uint32_t);
    void update(const std::string&, const std::string&, uint64_t, uint64_t);
    std::string read(const std::string&);
    //std::string read(const std::string&, uint32_t, uint32_t);
    std::string read(const std::string&, uint64_t, uint64_t);
    //std::string read(const std::string&, uint32_t, uint32_t, bool);
	void append(const std::string&, const std::string&, uint64_t);
    std::string read(const std::string&, uint64_t, uint64_t, bool);
    std::string batch_read(const std::string&, uint32_t, std::vector< std::pair<uint64_t, uint64_t> >& );
    std::string read_metadata();
    void remove(const std::string&);
    bool format();
    Local_io();
   
  private:
    std::string disk_path;
};

}
