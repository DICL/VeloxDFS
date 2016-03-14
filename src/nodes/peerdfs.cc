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

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace boost::asio;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
PeerDFS::PeerDFS (Context& context) : Node (context) { 
  Settings& setted = context.settings;

  int port       = setted.get<int>("network.port_cache");
  size           = setted.get<vec_str>("network.nodes").size();
  disk_path      = setted.get<string>("path.scratch");

  network   = new AsyncNetwork<P2P>(this, context, 10, port);

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
void PeerDFS::insert (std::string k, std::string v) {
  int which_node = h(k) % size;

  if (which_node == id) {
    string file_path = disk_path + string("/") + k;
    ofstream file (file_path);
    file << v;
    file.close();

  } else {
    logger->info ("[DFS] Forwaring KEY: %s -> %d",k.c_str(), which_node);
    KeyValue kv (k, v);
    network->send(which_node, &kv);
  }
}
// }}}
// request {{{
void PeerDFS::request (std::string key, req_func f) {
 int idx = h(key) % size;

 if (idx != id) {
   KeyRequest k_req (key);
   k_req.set_origin (id);
   network->send (idx, &k_req);
   requested_blocks.insert ({key, f});

 } else {
  ifstream in (disk_path + string("/") + key);
  string value ((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());

  in.close();
  f(key, value);
 }
}
// }}}
// close {{{
void PeerDFS::close() { exit(EXIT_SUCCESS); }
// }}}
// process (KeyValue* m) {{{
template<> void PeerDFS::process (KeyValue* m) {
  string& key = m->key;

  int which_node = h(key) % size;
  if (which_node == id or m->destination == id)  {
    logger->info ("Instering key = %s", key.c_str());
    insert(key, m->value);
  }

  if (requested_blocks.find(key) !=requested_blocks.end()){
    logger->info ("Executing func");
    requested_blocks[key](key, m->value);
    requested_blocks.erase(key);
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

  KeyValue kv (key, value);
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
// on_read (Message*) {{{
void PeerDFS::on_read (Message* m) {
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
  }
}
// }}}
// on_connect {{{
void PeerDFS::on_connect () {
  connected = true;
  logger->info ("Network established id=%d", id);
}
// }}}
// on_disconnect {{{
void PeerDFS::on_disconnect () {

}
// }}}
// insert_file {{{
bool PeerDFS::insert_file (messages::FileInfo* f) {
 bool ret = directory.is_exist(f->file_name.c_str());

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
  string key = m->block_name;
  directory.insert_block_metadata(*m);
  insert(key, m->content);
  return true;
}
// }}}
// Delete {{{
void PeerDFS::Delete (std::string k) {
  string file_path = disk_path + string("/") + k;
  remove(file_path.c_str());
}
// }}}
// delete_block {{{
bool PeerDFS::delete_block (messages::BlockDel* m) {
  string file_name = m->file_name;
  unsigned int block_seq = m->block_seq;
  string key = m->block_name;
  Delete(key);
  directory.delete_block_metadata(file_name, block_seq);
  return true;
}
// }}}
// delete_file {{{
bool PeerDFS::delete_file (messages::FileDel* f) {
  bool ret = directory.is_exist(f->file_name.c_str());

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
// request_block {{{
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
    fd.nodes.push_back(block_name);
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
}
