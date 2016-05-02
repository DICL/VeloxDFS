// includes & usings {{{
#include "peerdfs.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"
#include "../messages/keyrequest.hh"

#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
PeerDFS::PeerDFS (Network* net) : Node () { 
  network = net;

  int size = context.settings.get<vec_str>("network.nodes").size();
  boundaries.reset( new Histogram {size, 0});
  boundaries->initialize();

  directory.init_db();
}

PeerDFS::~PeerDFS() { }
// }}}
// insert {{{
void PeerDFS::insert (uint32_t hash_key, std::string name, std::string v) {
  int which_node = boundaries->get_index(hash_key);

  if (which_node == id) {
    INFO("[DFS] Saving locally KEY: %s", name.c_str());
    local_io.write(name, v);

  } else {
    INFO("[DFS] Forwaring KEY: %s -> %d", name.c_str(), which_node);
    KeyValue kv (hash_key, name, v);
    network->send(which_node, &kv);
  }
}
// }}}
// request {{{
void PeerDFS::request (uint32_t key, string name , req_func f) {
 int idx = boundaries->get_index(key);

 if (idx != id) {
   KeyRequest k_req (name);
   k_req.set_origin (id);
   network->send (idx, &k_req);
   requested_blocks.insert ({name, f});

 } else {
  string value = local_io.read(name);
  f(name, value);
 }
}
// }}}
// close {{{
void PeerDFS::close() { exit(EXIT_SUCCESS); }
// }}}
// insert_key {{{
void PeerDFS::insert_key (uint32_t key, std::string name, std::string value) {
  int which_node = boundaries->get_index(key);
  if (which_node == id)  {
    logger->info ("Instering key = %s", name.c_str());
    insert(key, name, value);
  }

  if (requested_blocks.find(name) != requested_blocks.end()){
    logger->info ("Executing func");
    requested_blocks[name](name, value);
    requested_blocks.erase(name);
  }
}
// }}}
// request_key {{{
void PeerDFS::request_key (std::string key, int origin) {
  INFO("Arrived req key = %s", key.c_str());
  string value = local_io.read(key);

  KeyValue kv (0, key, value);
  kv.destination = origin;
  network->send(origin, &kv);
}
// }}}
// insert_file {{{
bool PeerDFS::insert_file (messages::File* f) {
 bool ret = directory.file_exist(f->file_name.c_str());

 if (ret) {
   INFO("File:%s exists in db, ret = %i", f->file_name.c_str(), ret);
   return false;
 }

 directory.insert_file_metadata(*f);

 INFO("Saving to SQLite db");
 return true;
}
// }}}
// insert_block {{{
bool PeerDFS::insert_block (messages::Block* m) {
  string key = m->block_name;
  directory.insert_block_metadata(*m);
  insert(m->block_hash_key, key, m->content);
  return true;
}
// }}}
// delete_block {{{
bool PeerDFS::delete_block (messages::Block* m) {
  string file_name = m->file_name;
  unsigned int block_seq = m->block_seq;
  string key = m->block_name;
  local_io.remove(key);
  directory.delete_block_metadata(file_name, block_seq);
  return true;
}
// }}}
// delete_file {{{
bool PeerDFS::delete_file (std::string file_name) {
  bool ret = directory.file_exist(file_name.c_str());

  if (!ret) {
    INFO("File:%s doesn't exist in db, ret = %i", file_name.c_str(), ret);
    return false;
  }
 
  directory.delete_file_metadata(file_name);
 
  logger->info ("Removing from SQLite db");
  return true;
}
// }}}
// request_file {{{
FileDescription PeerDFS::request_file (std::string file_name) {
  File fi;
  fi.num_block = 0;
  FileDescription fd;
  fd.file_name  = file_name;

  directory.select_file_metadata(file_name, &fi);

  int num_blocks = fi.num_block;
  for (int i = 0; i< num_blocks; i++) {
    Block bi;
    directory.select_block_metadata (file_name, i, &bi);
    string block_name = bi.block_name;
    fd.blocks.push_back(block_name);
    fd.hash_keys.push_back(bi.block_hash_key);
  }

  return fd;
}
// }}}
// list {{{
bool PeerDFS::list (messages::List_files* m) {
  directory.select_all_file_metadata(m->data);
  return true;
}
// }}}
// format {{{
bool PeerDFS::format () {
  INFO("Formating DFS");
  local_io.format();
  directory.init_db();
  return true;
}
// }}}
// file_exist {{{
// FIXME need to think better name for this function
/**
 *@brief check we have given file name on our database
 *@param f is file name
 *@return return true if found that file on database otherwise return false
 */
bool PeerDFS::file_exist (std::string file_name) {
  return directory.file_exist(file_name.c_str());
}
// }}}
}
