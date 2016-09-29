#pragma once
#include "directory.hh"
#include "../common/context_singleton.hh"
#include "../common/hash.hh"
#include "../common/histogram.hh"

// common
#include "../messages/boost_impl.hh"
#include "../messages/checkexist.hh"
#include "../messages/factory.hh"
#include "../messages/reply.hh"

// file
#include "../messages/filedescription.hh"
#include "../messages/fileinfo.hh"
#include "../messages/filelist.hh"
#include "../messages/filerequest.hh"

// block
#include "../messages/blockdel.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockrequest.hh"
#include "../messages/blockupdate.hh"

// lib
#include <boost/asio.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

namespace eclipse {
  using namespace messages;
  using boost::asio::ip::tcp;
  class DFS {
    private:
      uint64_t BLOCK_SIZE;
      std::vector<std::string> nodes;
      boost::asio::io_service& iosvc;
      int replica;
      int port;
      int NUM_NODES;
      const int header_size = 16;
      const uint32_t KB = 1024;
      const uint32_t MB = 1024 * 1024;
      const uint64_t GB = (uint64_t) 1024 * 1024 * 1024;
      const uint64_t TB = (uint64_t) 1024 * 1024 * 1024 * 1024;
      const uint64_t PB = (uint64_t) 1024 * 1024 * 1024 * 1024 * 1024;
      const uint32_t K = 1000;
      const uint32_t M = 1000 * 1000;
      const uint64_t G = (uint64_t) 1000 * 1000 * 1000;
      const uint64_t T = (uint64_t) 1000 * 1000 * 1000 * 1000;
      const uint64_t P = (uint64_t) 1000 * 1000 * 1000 * 1000 * 1000;

      unique_ptr<tcp::socket> connect (uint32_t);
      void send_message(tcp::socket*, eclipse::messages::Message*);
      template <typename T>
        auto read_reply(tcp::socket*);
      bool check_local_exist(std::string);
      bool check_exist(std::string, std::string, uint32_t);

    public:
      DFS();
      void load_settings ();
      int put(int, char**);
      int get(int, char**);
      int cat(int, char**);
      int ls(int, char**);
      int rm(int, char**);
      int format(int, char**);
      int update(int, char**);
      /*
         int show(int, char**);
         int pget(int, char**);
         int append(int, char**);
         int sput(int, char**);
         int sget(int, char**);
         int yank(int, char**);
         int paste(int, char**);*/
  };
}
