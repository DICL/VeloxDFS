// includes & usings {{{
#include "peerdfs.hh"
#include "../network/asyncnetwork.hh"
#include "../network/p2p.hh"
#include "../common/settings.hh"
#include "../common/definitions.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"

#include <boost/range/adaptor/map.hpp>
#include <boost/asio.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <fstream>
#include <cstdio>
#include <dirent.h>
#include <unistd.h>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace boost::asio;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
PeerDFS::PeerDFS () : Node () { 
  Settings& setted = context.settings;

  int port       = setted.get<int>("network.ports.internal");
  size           = setted.get<vec_str>("network.nodes").size();
  disk_path      = setted.get<string>("path.scratch");
  replica        = setted.get<int>("filesystem.replica");
  nodes          = setted.get<vector<string>>("network.nodes");

  network   = new AsyncNetwork<P2P>(this, port);
  boundaries.reset( new Histogram {size, 0});
  boundaries->initialize();

  directory.init_db();
}

PeerDFS::~PeerDFS() { }
// }}}
// establish {{{
bool PeerDFS::establish () {
 logger->info ("Running Eclipse id=%d", id);
 network->establish();

 while (not connected) sleep(1);
 return true;
}
// }}}
// insert {{{
void PeerDFS::insert (uint32_t hash_key, std::string name, std::string v) {
  int which_node = boundaries->get_index(hash_key);

  if (which_node == id) {
    logger->info ("[DFS] Saving locally KEY: %s", name.c_str());
    string file_path = disk_path + string("/") + name;
    ofstream file (file_path);
    file << v;
    file.close();
  } else {
    logger->info ("[DFS] Forwaring KEY: %s -> %d", name.c_str(), which_node);
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
  ifstream in (disk_path + string("/") + name);
  string value ((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());

  in.close();
  f(name, value);
 }
}
// }}}
// close {{{
void PeerDFS::close() { exit(EXIT_SUCCESS); }
// }}}
// process (KeyValue* m) {{{
template<> void PeerDFS::process (KeyValue* m) {
  auto key = m->key;
  auto name =  m->name;

  int which_node = boundaries->get_index(key);
  if (which_node == id or m->destination == id)  {
    logger->info ("Instering key = %s", name.c_str());
    insert(key, m->name, m->value);
  }

  if (requested_blocks.find(name) != requested_blocks.end()){
    logger->info ("Executing func");
    requested_blocks[name](name, m->value);
    requested_blocks.erase(name);
  }
}
// }}}
// process (KeyRequest* m) {{{
template<> void PeerDFS::process (KeyRequest* m) {
  logger->info ("Arrived req key = %s", m->key.c_str());
  string& key = m->key;
  string value;

  ifstream in (disk_path + string("/") + key);
  in >> value;
  in.close();

  KeyValue kv (0, key, value);
  kv.destination = m->origin;
  network->send(m->origin, &kv);
}
// }}}
// process (Control* m) {{{
template<> void PeerDFS::process (Control* m) {
  switch (m->type) {
    case messages::SHUTDOWN:
      this->close();
      break;

    case messages::RESTART:
      break;
  }
}
// }}}
// process (BlockInfo* m) {{{
template<> void PeerDFS::process (BlockInfo* m) {
  string file_path = disk_path + string("/") + m->block_name;
  ofstream file (file_path);
  file << m->content;
  file.close();
  logger->info("ideal host = %s", m->node.c_str());
  logger->info("real host = %d", id);
}
// }}}
// process (BlockDel* m) {{{
template<> void PeerDFS::process (BlockDel* m) {
  string block_name = m->block_name;
  string block_path = disk_path + string("/") + block_name;
  remove(block_path.c_str());
}
// }}}
// on_read (Message*) {{{
void PeerDFS::on_read (Message* m, int) {
  string type = m->get_type();

  if (type == "KeyValue") {
    auto m_ = dynamic_cast<KeyValue*>(m);
    process(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process(m_);

  } else if (type == "KeyRequest") {
    auto m_ = dynamic_cast<KeyRequest*>(m);
    process(m_);

  } else if (type == "BlockInfo") {
    auto m_ = dynamic_cast<BlockInfo*>(m);
    process(m_);
  } else if (type == "BlockDel") {
    auto m_ = dynamic_cast<BlockDel*>(m);
    process(m_);
  }
}
// }}}
// {{{ on_connect
void PeerDFS::on_connect () {
  connected = true;
  logger->info ("Network established id=%d", id);
}
// }}}
// on_disconnect {{{
void PeerDFS::on_disconnect (int id) {
}
// }}}
// insert_file {{{
bool PeerDFS::insert_file (messages::FileInfo* f) {
 bool ret = directory.file_exist(f->file_name.c_str());

 if (ret) {
   logger->info ("File:%s exists in db, ret = %i", f->file_name.c_str(), ret);
   return false;
 }

 directory.insert_file_metadata(*f);

 logger->info ("Saving to SQLite db");
 return true;
}
// }}}
// insert_block {{{
bool PeerDFS::insert_block (messages::BlockInfo* m) {
  directory.insert_block_metadata(*m);
  int which_node = boundaries->get_index(m->block_hash_key);
  int tmp_node = which_node;
  for(int i=0; i<replica; i++)
  {
    if(i%2 == 1)
    {
      tmp_node = (which_node + (i+1)/2 + nodes.size()) % nodes.size();
    }
    else
    {
      tmp_node = (which_node - i/2 + nodes.size()) % nodes.size();
    }
    uint32_t tmp_hash_key = boundaries->random_within_boundaries(tmp_node);
    insert(tmp_hash_key, m->block_name, m->content);
    sleep(1);
  }
  return true;
}
// }}}
// delete_block {{{
bool PeerDFS::delete_block (messages::BlockDel* m) {
  directory.delete_block_metadata(m->file_name, m->block_seq);
  int which_node = boundaries->get_index(m->block_hash_key);
  int tmp_node = which_node;
  
  for(int i=0; i<replica; i++)
  {
    if(i%2 == 1)
    {
      tmp_node = (which_node + (i+1)/2 + nodes.size()) % nodes.size();
    }
    else
    {
      tmp_node = (which_node - i/2 + nodes.size()) % nodes.size();
    }
    if(id == tmp_node)
    {
      string block_name = m->block_name;
      string block_path = disk_path + string("/") + block_name;
      remove(block_path.c_str());
    }
    else
    {
      network->send(tmp_node, m);
    }
  }
  return true;
}
// }}}
// delete_file {{{
bool PeerDFS::delete_file (messages::FileDel* f) {
  bool ret = directory.file_exist(f->file_name.c_str());

  if (!ret) {
    logger->info ("File:%s doesn't exist in db, ret = %i", f->file_name.c_str(),
        ret);
    return false;
  }
 
  directory.delete_file_metadata(f->file_name);
 
  logger->info ("Removing from SQLite db");
  return true;
}
// }}}
// request_file {{{
FileDescription PeerDFS::request_file (messages::FileRequest* m) {
  string file_name = m->file_name;

  FileInfo fi;
  fi.num_block = 0;
  FileDescription fd;
  fd.file_name  = file_name;

  directory.select_file_metadata(file_name, &fi);

  int num_blocks = fi.num_block;
  for (int i = 0; i< num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata (file_name, i, &bi);
    string block_name = bi.block_name;
    fd.blocks.push_back(block_name);
    fd.hash_keys.push_back(bi.block_hash_key);
  }

  return fd;
}
// }}}
// list {{{
bool PeerDFS::list (messages::FileList* m) {
  directory.select_all_file_metadata(m->data);
  return true;
}
// }}}
// format {{{
bool PeerDFS::format () {
  logger->info ("Formating DFS");

  string fs_path = context.settings.get<string>("path.scratch");
  string md_path = context.settings.get<string>("path.metadata");

  DIR *theFolder = opendir(fs_path.c_str());
  struct dirent *next_file;
  char filepath[256] = {0};

  while ( (next_file = readdir(theFolder)) != NULL ) {
    sprintf(filepath, "%s/%s", fs_path.c_str(), next_file->d_name);
    remove(filepath);
  }
  closedir(theFolder);

  remove((md_path + "/metadata.db").c_str());
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
