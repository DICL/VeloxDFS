#pragma once
#include "../common/hash.hh"
#include "../common/context.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "../messages/fileexist.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/blockdel.hh"
#include "../messages/filedescription.hh"
#include "../messages/reply.hh"
#include "../messages/blockrequest.hh"
#include "directory.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>
#include <boost/asio.hpp>
#include <stdint.h>
#include <ctime>

namespace eclipse {
  using namespace messages;
  using boost::asio::ip::tcp;
  class DFS {
    private:
      uint32_t BLOCK_SIZE;
      uint32_t NUM_SERVERS;
      std::string path;
      boost::asio::io_service iosvc;
      int replica;
      int port;
      std::vector<std::string> nodes;

      unique_ptr<tcp::socket> connect (uint32_t);
      void send_message (tcp::socket*, eclipse::messages::Message*);
      template <typename T>
      auto read_reply (tcp::socket*);

    public:
      void load_settings ();
      int put (int argc, char* argv[]);
      int get (int argc, char* argv[]);
      int ls (int argc, char* argv[]);
      int rm (int argc, char* argv[]);
      int format (int argc, char* argv[]);
      int show (int argc, char* argv[]);
  };
}
