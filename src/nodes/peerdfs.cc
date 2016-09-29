#include "peerdfs.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"
#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace boost::asio;
using namespace std;

namespace eclipse {
  // PeerDFS {{{
  PeerDFS::PeerDFS(Network* net) : Node() { 
    network = net;
    net->attach(this);
    //NUM_NODES = context.settings.get<vec_str>("network.nodes").size();
  }
  // }}}
  // update {{{
  void PeerDFS::update(int net_id, string name, string& value, uint64_t pos, uint64_t len) {
    if (net_id == id) {
      INFO("[DFS_INFO] Updating locally (KEY: %s)", name.c_str());
      local_io.update(name, value, pos, len);
    } else {
      INFO("[DFS_INFO] Forwaring (KEY: %s -> %d)", name.c_str(), net_id);
      OffsetKeyValue okv(net_id, name, value, pos, len);
      network->send(net_id, &okv);
    }
  }
  // }}}
  // request {{{
  void PeerDFS::request(int net_id, string name , req_func rf) {
    if (net_id != id) {
      KeyRequest k_req(name);
      k_req.set_origin(id);
      network->send(net_id, &k_req);
      requested_blocks.insert({name, rf});
    } else {
      string value = local_io.read(name);
      rf(name, value);
    }
  }
  // }}}
  // close {{{
  void PeerDFS::close() { exit(EXIT_SUCCESS); }
  // }}}
  // process (OffsetKeyValue* m) {{{
  template<> void PeerDFS::process(OffsetKeyValue* m) {
    if (m->key == id or m->destination == id)  {
      INFO("[DFS_INFO] Updating (KEY: %s)", m->name.c_str());
      update(m->key, m->name, m->value, m->pos, m->len);
    }
  }
  // }}}
  // process (KeyRequest* m) {{{
  template<> void PeerDFS::process (KeyRequest* m) {
    INFO("[DFS_INFO] Arrived req (KEY: %s)", m->key.c_str());
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
  // process (MetaData* m) {{{
  template<> void PeerDFS::process(MetaData* m) {
    std::string file_name = m->node + "_replica";
    local_io.write(file_name, m->content);
  }
  // }}}
  // process (BlockInfo* m) {{{
  template<> void PeerDFS::process(BlockInfo* m) {
    NUM_NODES = context.settings.get<vec_str>("network.nodes").size();
    vector<int> nodes;
    for (int i=0; i<m->replica; i++) {
      int net_id;
      if (i%2 == 1) {
        net_id = (id + (i+1)/2 + NUM_NODES) % NUM_NODES;
      } else {
        net_id = (id - i/2 + NUM_NODES) % NUM_NODES;
      }
      if ((int)((m->net_id + m->seq) % NUM_NODES) == id && i > 0) {
        nodes.push_back(net_id);
      }
    }
    local_io.write(m->name, m->content);
    if ((int)((m->net_id + m->seq) % NUM_NODES) == id) {
      if (m->replica > 1) {
        network->send_and_replicate(nodes, m);
      }
    }
    //INFO("[DFS_INFO] <processBI> Block inserted (NET_ID: %d, NAME: %s)", id, m->name.c_str());
  }
  // }}}
  // process (BlockUpdate* m) {{{
  template<> void PeerDFS::process(BlockUpdate* m) {
    local_io.update(m->name, m->content, m->pos, m->len);
    logger->info("[DFS_INFO] Block updating (NET_ID: %d)", id);
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
    if (type == "BlockDel") {
      auto m_ = dynamic_cast<BlockDel*>(m);
      process(m_);
    } else if (type == "BlockInfo") {
      auto m_ = dynamic_cast<BlockInfo*>(m);
      process(m_);
    } else if (type == "BlockUpdate") {
      auto m_ = dynamic_cast<BlockUpdate*>(m);
      process(m_);
    } else if (type == "Control") {
      auto m_ = dynamic_cast<Control*>(m);
      process(m_);
    } else if (type == "KeyRequest") {
      auto m_ = dynamic_cast<KeyRequest*>(m);
      process(m_);
    } else if (type == "MetaData") {
      auto m_ = dynamic_cast<MetaData*>(m);
      process(m_);
    } else if (type == "OffsetKeyValue") {
      auto m_ = dynamic_cast<OffsetKeyValue*>(m);
      process(m_);
    }
  }
  // }}}
  // on_connect {{{
  void PeerDFS::on_connect() {
    INFO("[DFS_INFO] Network established (NET_ID: %d)", id);
  }
  // }}}
  // on_disconnect {{{
  void PeerDFS::on_disconnect(int id) {
  }
  // }}}
  // delete_file {{{
  bool PeerDFS::delete_file (FileDel* f) {
    if (directory.check_exist(f->name.c_str(), "file") == true) {
      directory.delete_file_metadata(f->name);
      directory.delete_block_metadata(f->name);
      directory.delete_node_metadata(f->name);
      //replicate_metadata();
      INFO("[DFS_INFO] <delete_file> Removing from SQLite db (NAME: %s)", f->name.c_str());
      return true;
    } else {
      INFO("[DFS_WARN] <delete_file> File doesn't exist in db (NAME: %s)", f->name.c_str());
      return false;
    }
  }
  // }}}
  // insert_file {{{
  bool PeerDFS::insert_file(FileInfo* f) {
    if (directory.check_exist(f->name.c_str(), "file") == true) {
      INFO("[DFS_WARN] File exists in db (NAME: %s)", f->name.c_str());
      return false;
    } else {
      directory.insert_file_metadata(f);
      //replicate_metadata();
      INFO("[DFS_INFO] Saving to SQLite db (NAME: %s)", f->name.c_str());
      return true;
    }
  }
  // }}}
  // request_file {{{
  FileDescription PeerDFS::request_file (FileRequest* m) {
    FileInfo fi;
    directory.select_file_metadata(m->name, &fi);
    vector<BlockInfo> bl;
    directory.select_block_metadata(fi.name, &bl);
    FileDescription fd;
    fd.name = fi.name;
    fd.size = fi.size;
    fd.replica = fi.replica;
    vector<int> net_id;
    for (vector<BlockInfo>::iterator it=bl.begin(); it!=bl.end(); ++it) {
      fd.block_name.push_back(it->name);
      fd.block_size.push_back(it->size);
      NodeInfo ni;
      directory.select_one_node_metadata(it->name, &ni);
      fd.net_id.push_back(ni.net_id);
    }
    return fd;
  }
  // }}}
  // update_file {{{
  bool PeerDFS::update_file(FileUpdate* f) {
    if (directory.check_exist(f->name.c_str(), "file") ==  true) {
      directory.update_file_metadata(f);
      INFO("[DFS_INFO] Updating to SQLite db (NAME: %s)", f->name.c_str());
      return true;
    } else {
      INFO("[DFS_WARN] File doesn't exist in db (NAME: %s)", f->name.c_str());
      return false;
    }
  }
  // }}}
// delete_block {{{
bool PeerDFS::delete_block(BlockDel* m) {
  local_io.remove(m->name);
  //replicate_metadata();
  return true;
}
// }}}
  // insert_block {{{
  bool PeerDFS::insert_block(BlockInfo* m) {
    NUM_NODES = context.settings.get<vec_str>("network.nodes").size();
    vector<int> nodes;
    for (int i=0; i<m->replica; i++) {
      int net_id;
      if (i%2 == 1) {
        net_id = (m->net_id + m->seq + (i+1)/2 + NUM_NODES) % NUM_NODES;
      } else {
        net_id = (m->net_id + m->seq - i/2 + NUM_NODES) % NUM_NODES;
      }
      NodeInfo ni(m->name, m->file_name, i, net_id);
      directory.insert_node_metadata(&ni);
      if ((int)((m->net_id + m->seq) % NUM_NODES) == id && i > 0) {
        nodes.push_back(net_id);
      }
    }
    if ((int)((m->net_id + m->seq) % NUM_NODES) == id) {
      //INFO("[DFS_INFO] <insert_block> Saving locally (NAME: %s)", m->name.c_str());
      local_io.write(m->name, m->content);
      if (m->replica > 1) {
        network->send_and_replicate(nodes, m);
      }
    } else {
      //INFO("[DFS_INFO] <insert_block> Saving remotely (NAME: %s)", m->name.c_str());
      //network->send_and_replicate(nodes, m);
      network->send((m->net_id + m->seq) % NUM_NODES, m);
    }
    //replicate_metadata();
    directory.insert_block_metadata(m);
    INFO("[DFS_INFO] Block inserted (NAME: %s)", m->name.c_str());
    return true;
  }
  // }}}
  // update_block {{{
bool PeerDFS::update_block(BlockUpdate* m) {
  NUM_NODES = context.settings.get<vec_str>("network.nodes").size();
  /*
     for (int i=0; i<m->replica; i++) {
     int net_id;
     if(i%2 == 1) {
     which_node = (net_id + (i+1)/2 + NUM_NODES) % NUM_NODES;
     } else {
     which_node = (net_id - i/2 + NUM_NODES) % NUM_NODES;
     }
     INFO("[DFS_INFO] Updating locally (NAME: %s)", m->name.c_str());
     update(which_node, m->name, m->content, m->pos, m->len);
     }
     } else {
     }*/
  update(m->net_id, m->name, m->content, m->pos, m->len);
  directory.update_block_metadata(m);
  INFO("[DFS_INFO] Updating block (NAME: %s)", m->name.c_str());
  return true;
}
// }}}
// request_node {{{
NodeDescription PeerDFS::request_node (NodeRequest* m) {
  vector<NodeInfo> nl;
  directory.select_rm_node_metadata(m->name, &nl);
  NodeDescription nd;
  nd.node_list.assign(nl.begin(), nl.end());
  return nd;
}
// }}}
// list {{{
bool PeerDFS::list (FileList* m) {
  directory.select_all_file_metadata(&m->data);
  return true;
}
// }}}
// format {{{
bool PeerDFS::format () {
  INFO("[DFS_INFO] Formating DFS");
  local_io.format();
  directory.init_db();
  return true;
}
// }}}
// check_exist {{{
bool PeerDFS::check_exist (string name, string type) {
  INFO("[PeerDFS] check_exist (NAME: %s)", name.c_str());
  return directory.check_exist(name.c_str(), type.c_str());
}
// }}}
// replicate_metadata {{{
// This function replicates to its right and left neighbor
// node the metadata db. This function is intended to be 
// invoked whenever the metadata db is modified.
void PeerDFS::replicate_metadata() {
    NUM_NODES = context.settings.get<vec_str>("network.nodes").size();
  MetaData md; 
  md.node = context.settings.getip();
  md.content = local_io.read_metadata();

  int left_node = ((id - 1) < 0) ? NUM_NODES - 1: id - 1;
  int right_node = ((id + 1) == NUM_NODES) ? 0 : id + 1;

  network->send(left_node, &md);
  network->send(right_node, &md);
}
// }}}
}
