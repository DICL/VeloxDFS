#include "remotedfs.hh"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
namespace ph = std::placeholders;

// Constructor {{{
RemoteDFS::RemoteDFS(PeerDFS* p, network::Network* net) : Router(net) {
  peer_dfs = p;
  using namespace std::placeholders;
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto& rt = routing_table;
  rt.insert({"FormatRequest", bind(&RemoteDFS::request_format, this, _1, _2)});
  rt.insert({"CheckExist", bind(&RemoteDFS::check_exist, this, _1, _2)});
  rt.insert({"FileDel", bind(&RemoteDFS::delete_file, this, _1, _2)});
  rt.insert({"FileInfo",   bind(&RemoteDFS::insert_file, this, _1, _2)});
  rt.insert({"FileList", bind(&RemoteDFS::request_ls, this, _1, _2)});
  rt.insert({"FileRequest", bind(&RemoteDFS::request_file, this, _1, _2)});
  rt.insert({"FileUpdate",   bind(&RemoteDFS::update_file, this, _1, _2)});
  rt.insert({"BlockDel", bind(&RemoteDFS::delete_block, this, _1, _2)});
  rt.insert({"BlockInfo",  bind(&RemoteDFS::insert_block, this, _1, _2)});
  rt.insert({"BlockRequest", bind(&RemoteDFS::request_block, this, _1, _2)});
  rt.insert({"BlockUpdate",  bind(&RemoteDFS::update_block, this, _1, _2)});
  rt.insert({"NodeRequest",  bind(&RemoteDFS::request_node, this, _1, _2)});
}
// }}}
// check_exist {{{
void RemoteDFS::check_exist (Message* m_, int n_channel) {
  auto m = dynamic_cast<CheckExist*> (m_);
  bool ret = peer_dfs->check_exist(m->name, m->type);
  Reply reply;
  if (ret) {
    reply.message = "TRUE";
  } else {
    reply.message = "FALSE";
  }
  network->send(n_channel, &reply);
}
// }}}
// request_format {{{
void RemoteDFS::request_format (Message* m_, int n_channel) {
  //INFO("[RemoteDFS_INFO] FormatRequest received");
  Reply reply;
  if (peer_dfs->format()) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
  }
  network->send(n_channel, &reply);
}
// }}}
// request_ls {{{
void RemoteDFS::request_ls (Message* m_, int n_channel) {
  auto m = dynamic_cast<FileList*> (m_);
  //INFO("[RemoteDFS_INFO] FileList received");
  peer_dfs->list(m);
  network->send(n_channel, m);
}
// }}}
// delete_file {{{
void RemoteDFS::delete_file(Message* m_, int n_channel) {
  auto m = dynamic_cast<FileDel*> (m_); 
  //INFO("[RemoteDFS_INFO] FileDel received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->delete_file(m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "File doesn't exist";
  }
  network->send(n_channel, &reply);
}
// }}}
// insert_file {{{
void RemoteDFS::insert_file(Message* m_, int n_channel) {
  auto m = dynamic_cast<FileInfo*> (m_);
  //INFO("[RemoteDFS_INFO] FileInfo received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->insert_file (m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "File already exists";
  }
  network->send(n_channel, &reply);
}
// }}}
// request_file {{{
void RemoteDFS::request_file(Message* m_, int n_channel) {
  auto m = dynamic_cast<FileRequest*>(m_);
  //INFO("[RemoteDFS_INFO] FileRequest received (NAME: %s)",m->name.c_str());
  auto fd = peer_dfs->request_file(m);
  network->send(n_channel, &fd);
}
// }}}
// update_file {{{
void RemoteDFS::update_file(Message* m_, int n_channel) {
  auto m = dynamic_cast<FileUpdate*> (m_);
  //INFO("[RemoteDFS_INFO] FileUpdate received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->update_file(m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "File doesn't exist";
  }
  network->send(n_channel, &reply);
}
// }}}
// delete_block {{{
void RemoteDFS::delete_block(Message* m_, int n_channel) {
  auto m = dynamic_cast<BlockDel*> (m_);
  INFO("[RemoteDFS_INFO] BlockDel received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->delete_block(m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "Block doesn't exist";
  }
  network->send(n_channel, &reply);
}
// }}}
// insert_block {{{
void RemoteDFS::insert_block(Message* m_, int n_channel) {
  auto m = dynamic_cast<BlockInfo*> (m_);
  //INFO("[RemoteDFS_INFO] BlockInfo received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->insert_block(m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "Block already exists";
  }
  network->send(n_channel, &reply);
}
// }}}
// send_block {{{
void RemoteDFS::send_block (std::string k, std::string v, int n_channel) {
  //INFO("[RemoteDFS_INFO] Sending block (NAME: %s)",k.c_str());
  BlockInfo bi;
  bi.name = k;
  bi.content = v;
  network->send(n_channel, &bi);
}
// }}}
// request_block {{{
void RemoteDFS::request_block (Message* m_, int n_channel) {
  auto m = dynamic_cast<BlockRequest*> (m_);
  //INFO("[RemoteDFS_INFO] BlockRequest received (NAME: %s)",m->name.c_str());
  int key = m->net_id;
  string name = m->name;
  peer_dfs->request(key, name, std::bind(&RemoteDFS::send_block, this, ph::_1, ph::_2, n_channel));
}
// }}}
// update_block {{{
void RemoteDFS::update_block(Message* m_, int n_channel) {
  auto m = dynamic_cast<BlockUpdate*> (m_);
  //INFO("[RemoteDFS_INFO] BlockUpdate received (NAME: %s)",m->name.c_str());
  bool ret = peer_dfs->update_block(m);
  Reply reply;
  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "Block update failed";
  }
  network->send(n_channel, &reply);
}
// }}}
// request_block {{{
void RemoteDFS::request_node (Message* m_, int n_channel) {
  auto m = dynamic_cast<NodeRequest*> (m_);
  //INFO("[RemoteDFS_INFO] NodeRequest received (NAME: %s)",m->name.c_str());
  auto nd = peer_dfs->request_node(m);
  network->send(n_channel, &nd);
}
// }}}
