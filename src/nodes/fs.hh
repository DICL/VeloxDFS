#pragma once

#include "../messages/file.hh"
#include "../messages/block.hh"
#include "../messages/filedescription.hh"
#include "../messages/list_files.hh"
#include <string>

namespace eclipse {
typedef std::function<void(std::string, std::string)> req_func;

struct FS {
  virtual bool insert_block (messages::Block*) = 0;
  virtual bool insert_file (messages::File*) = 0;
  virtual bool delete_block (messages::Block*) = 0;
  virtual bool delete_file (std::string) = 0;
  virtual bool list (messages::List_files*) = 0;
  virtual bool format () = 0;
  virtual messages::FileDescription request_file (std::string) = 0;
  virtual bool file_exist (std::string) = 0;
  virtual void insert_key (uint32_t, std::string, std::string) = 0;
  virtual void request_key (std::string, int) = 0;
  virtual void request (uint32_t, std::string, req_func) = 0;
};

}
