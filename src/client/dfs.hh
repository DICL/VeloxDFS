#pragma once
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <stdint.h>
#include <memory>

namespace velox {

using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

class DFS {
  public:
    DFS();
    void load_settings();
    int put(vec_str);
    int get(vec_str);
    int cat(vec_str);
    int ls(vec_str);
    int rm(vec_str);
    int format();
    int show(vec_str);
    int pget(vec_str);
    int update(vec_str);
    int append(vec_str);
    bool fexists(std::string);
    bool exists(std::string);
    bool touch(std::string);

    int push_back(vec_str);
    std::string load(std::string);

  private:
    uint32_t BLOCK_SIZE;
    uint32_t NUM_NODES;
    int replica;
    int port;
    std::vector<std::string> nodes;

    std::unique_ptr<tcp::socket> connect (uint32_t);
};

}
