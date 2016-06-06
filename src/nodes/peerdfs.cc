// includes & usings {{{
#include "peerdfs.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"

#include <boost/asio.hpp>
#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace boost::asio;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
PeerDFS::PeerDFS (Network* net) : Node () { 
  network = net;
  net->attach(this);

  size = context.settings.get<vec_str>("network.nodes").size();
  boundaries.reset( new Histogram {size, 0});
  boundaries->initialize();

  directory.init_db();
}

PeerDFS::~PeerDFS() { }
// }}}
// insert {{{
void PeerDFS::insert(uint32_t hash_key, std::string name, std::string v) {
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
void PeerDFS::request(uint32_t key, string name , req_func f) {
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
// process (KeyValue* m) {{{
template<> void PeerDFS::process(KeyValue* m) {
  auto key = m->key;
  auto name =  m->name;

  int which_node = boundaries->get_index(key);
  if (which_node == id or m->destination == id)  {
    INFO("Instering key = %s", name.c_str());
    insert(key, m->name, m->value);
  }

  if (requested_blocks.find(name) != requested_blocks.end()) {
    INFO("Executing func");
    requested_blocks[name](name, m->value);
    requested_blocks.erase(name);
  }
}
// }}}
// process (KeyRequest* m) {{{
template<> void PeerDFS::process (KeyRequest* m) {
  INFO("Arrived req key = %s", m->key.c_str());
  string value = local_io.read(m->key);

  KeyValue kv (0, m->key, value);
  kv.destination = m->origin;
  network->send(m->origin, &kv);
}
// }}}
// process (Control* m) {{{
template<> void PeerDFS::process(Control* m) {
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
template<> void PeerDFS::process(BlockInfo* m) {
  local_io.write(m->name, m->content);
  logger->info("ideal host = %s", m->node.c_str());
  logger->info("real host = %d", id);
}
// }}}
// process (BlockDel* m) {{{
template<> void PeerDFS::process (BlockDel* m) {
  local_io.remove(m->name);
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
// on_connect {{{
void PeerDFS::on_connect () {
  INFO("Network established id=%d", id);
}
// }}}
// on_disconnect {{{
void PeerDFS::on_disconnect(int id) {
}
// }}}
// insert_file {{{
bool PeerDFS::insert_file(messages::FileInfo* f) {
 bool ret = directory.file_exist(f->name.c_str());

 if (ret) {
   INFO("File:%s exists in db, ret = %i", f->name.c_str(), ret);
   return false;
 }

 directory.insert_file_metadata(*f);

 logger->info("Saving to SQLite db");
 return true;
}
// }}}
// insert_block {{{
bool PeerDFS::insert_block(messages::BlockInfo* m) {
  directory.insert_block_metadata(*m);
  int which_node = boundaries->get_index(m->hash_key);
  int tmp_node;
  for (int i=0; i<m->replica; i++) {
    if(i%2 == 1) {
      tmp_node = (which_node + (i+1)/2 + size) % size;
    } else {
      tmp_node = (which_node - i/2 + size) % size;
    }
    uint32_t tmp_hash_key = boundaries->random_within_boundaries(tmp_node);
    insert(tmp_hash_key, m->name, m->content);
  }
  return true;
}
// }}}
// delete_block {{{
bool PeerDFS::delete_block(messages::BlockDel* m) {
  directory.delete_block_metadata(m->file_name, m->seq);
  int which_node = boundaries->get_index(m->hash_key);
  int tmp_node;
  
  for (int i=0; i<m->replica; i++) {
    if (i%2 == 1) {
      tmp_node = (which_node + (i+1)/2 + size) % size;
    } else {
      tmp_node = (which_node - i/2 + size) % size;
    }
    if (id == tmp_node) {
      string block_name = m->name;
      local_io.remove(block_name);
    } else {
      network->send(tmp_node, m);
    }
  }
  return true;
}
// }}}
// delete_file {{{
bool PeerDFS::delete_file (messages::FileDel* f) {
  bool ret = directory.file_exist(f->name.c_str());
  if (!ret) {
    INFO("File:%s doesn't exist in db, ret = %i", f->name.c_str(),
        ret);
    return false;
  }
  directory.delete_file_metadata(f->name);
  INFO("Removing from SQLite db");
  return true;
}
// }}}
// request_file {{{
FileDescription PeerDFS::request_file (messages::FileRequest* m) {
  string file_name = m->name;

  FileInfo fi;
  fi.num_block = 0;
  FileDescription fd;
  fd.name  = file_name;

  directory.select_file_metadata(file_name, &fi);
  fd.replica = fi.replica;

  int num_blocks = fi.num_block;
  for (int i = 0; i< num_blocks; i++) {
    BlockInfo bi;
    directory.select_block_metadata (file_name, i, &bi);
    string block_name = bi.name;
    fd.blocks.push_back(block_name);
    fd.hash_keys.push_back(bi.hash_key);
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
  INFO("Formating DFS");
  local_io.format();
  directory.init_db();
  return true;
}
// }}}
// file_exist {{{
bool PeerDFS::file_exist (std::string file_name) {
  return directory.file_exist(file_name.c_str());
}
// }}}
}
