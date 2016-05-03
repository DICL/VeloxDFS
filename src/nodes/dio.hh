#pragma once

#include "node.hh"
#include "local_io.hh"
#include "../common/histogram.hh"

#include <string>

namespace eclipse {

using vec_str = std::vector<std::string>;
typedef std::function<void(std::string, std::string)> req_func;

class DIO: public Node {
  public:
    DIO (network::Network*);
    ~DIO ();

    void insert_key (uint32_t, std::string, std::string);
    void request_key (std::string, int);

    virtual void insert (uint32_t, std::string, std::string);
    virtual void request (uint32_t, std::string, req_func);
    void close ();

    Local_io local_io;

  protected:
    std::unique_ptr<Histogram> boundaries;
    std::map<std::string, req_func> requested_blocks;
};

}
