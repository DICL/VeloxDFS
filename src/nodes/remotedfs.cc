#include "remotedfs.hh"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
namespace ph = std::placeholders;

// Constructor {{{
RemoteDFS::RemoteDFS () : Router() {
  using namespace std::placeholders;
  using std::placeholders::_1;
  using std::placeholders::_2;
  auto& rt = routing_table;
  rt.insert({"BlockInfo",  bind(&RemoteDFS::insert_block, this, _1, _2)});
  rt.insert({"FileInfo",   bind(&RemoteDFS::insert_file, this, _1, _2)});
  rt.insert({"FileRequest", bind(&RemoteDFS::request_file, this, _1, _2)});
  rt.insert({"BlockRequest", bind(&RemoteDFS::request_block, this, _1, _2)});
  rt.insert({"FileList", bind(&RemoteDFS::request_ls, this, _1, _2)});
  rt.insert({"BlockDel", bind(&RemoteDFS::delete_block, this, _1, _2)});
  rt.insert({"FileDel", bind(&RemoteDFS::delete_file, this, _1, _2)});
  rt.insert({"FormatRequest", bind(&RemoteDFS::request_format, this, _1, _2)});
  rt.insert({"FileExist", bind(&RemoteDFS::file_exist, this, _1, _2)});
}
// }}}
// establish {{{
bool RemoteDFS::establish () {
  peer  = make_unique<PeerDFS> ();
  peer_dfs = dynamic_cast<PeerDFS*> (peer.get());
  peer_dfs->establish();
  Router::establish();
  return true;
}
// }}}
// BlockInfo {{{
void RemoteDFS::insert_block (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::BlockInfo*> (m_);
  logger->info ("BlockInfo received");

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
// delete_block {{{
// }}}
void RemoteDFS::delete_block (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::BlockDel*> (m_);
  logger->info ("BlockDel received");

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
// FileInfo* {{{
void RemoteDFS::insert_file (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::FileInfo*> (m_);
  logger->info ("FileInfo received");

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
void RemoteDFS::delete_file (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::FileDel*> (m_);
  logger->info ("FileDel received");

  bool ret = peer_dfs->delete_file (m);
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
// request_file {{{
void RemoteDFS::request_file (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::FileRequest*> (m_);
  logger->info ("File Info received %s", m->file_name.c_str());

  auto fd = peer_dfs->request_file (m);
  network->send(n_channel, &fd);
}
// }}}
// request_block {{{
void RemoteDFS::request_block (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::BlockRequest*> (m_);
  auto key = m->hash_key;
  auto name= m->block_name;
  peer_dfs->request(key, name, std::bind(&RemoteDFS::send_block, this, 
        ph::_1, ph::_2, n_channel));
}
// }}}
// request_ls {{{
void RemoteDFS::request_ls (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::FileList*> (m_);
  peer_dfs->list(m);
  network->send(n_channel, m);
}
// }}}
// send_block {{{
void RemoteDFS::send_block (std::string k, std::string v, int n_channel) {
  logger->info ("Sending Block %s", k.c_str());
  BlockInfo bi;
  bi.block_name = k;
  bi.content = v;

  network->send(n_channel, &bi);
}
// }}}
// request_format {{{
void RemoteDFS::request_format (messages::Message* m_, int n_channel) {
  bool ret = peer_dfs->format();
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
  }

  network->send(n_channel, &reply);
}
// }}}
// file_exist {{{
void RemoteDFS::file_exist (messages::Message* m_, int n_channel) {
  auto m = dynamic_cast<messages::FileExist*> (m_);
  bool ret = peer_dfs->file_exist(m->file_name);
  Reply reply;

  if (ret) {
    reply.message = "TRUE";

  } else {
    reply.message = "FALSE";
  }
  network->send(n_channel, &reply);
}
// }}}
