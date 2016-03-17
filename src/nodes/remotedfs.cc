#include "remotedfs.hh"
#include <functional>
#include <map>

using namespace std;
using namespace eclipse;
namespace ph = std::placeholders;

// Constructor {{{
RemoteDFS::RemoteDFS (Context& c) : Router(c), peer(c) {
  routing_table.insert({"BlockInfo",  bind(&RemoteDFS::insert_block, this, ph::_1)});
  routing_table.insert({"FileInfo",   bind(&RemoteDFS::insert_file, this, ph::_1)});
  routing_table.insert({"FileRequest", bind(&RemoteDFS::request_file, this, ph::_1)});
  routing_table.insert({"BlockRequest", bind(&RemoteDFS::request_block, this, ph::_1)});
  routing_table.insert({"FileList", bind(&RemoteDFS::request_ls, this, ph::_1)});
  routing_table.insert({"BlockDel", bind(&RemoteDFS::delete_block, this, ph::_1)});
  routing_table.insert({"FileDel", bind(&RemoteDFS::delete_file, this, ph::_1)});
  routing_table.insert({"FormatRequest", bind(&RemoteDFS::request_format, this, ph::_1)});
}
// }}}
// establish {{{
bool RemoteDFS::establish () {
  peer.establish();
  Router::establish();
  return true;
}
// }}}
// BlockInfo {{{
void RemoteDFS::insert_block (messages::Message* m_) {
  auto m = dynamic_cast<messages::BlockInfo*> (m_);
  logger->info ("BlockInfo received");

  bool ret = peer.insert_block(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "Block already exists";
  }

  network->send(0, &reply);
}
void RemoteDFS::delete_block (messages::Message* m_) {
  auto m = dynamic_cast<messages::BlockDel*> (m_);
  logger->info ("BlockDel received");

  bool ret = peer.delete_block(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "Block doesn't exist";
  }

  network->send(0, &reply);
}

// }}}
// FileInfo* {{{
void RemoteDFS::insert_file (messages::Message* m_) {
  auto m = dynamic_cast<messages::FileInfo*> (m_);
  logger->info ("FileInfo received");

  bool ret = peer.insert_file (m);
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "File already exists";
  }

  network->send(0, &reply);
}
void RemoteDFS::delete_file (messages::Message* m_) {
  auto m = dynamic_cast<messages::FileDel*> (m_);
  logger->info ("FileDel received");

  bool ret = peer.delete_file (m);
  Reply reply;

  if (ret) {
    reply.message = "OK";
  } else {
    reply.message = "FAIL";
    reply.details = "File doesn't exist";
  }

  network->send(0, &reply);
}
// }}}
// request_file {{{
void RemoteDFS::request_file (messages::Message* m_) {
  auto m = dynamic_cast<messages::FileRequest*> (m_);
  logger->info ("File Info received %s", m->file_name.c_str());

  auto fd = peer.request_file (m);
  network->send(0, &fd);
}
// }}}
// request_block {{{
void RemoteDFS::request_block (messages::Message* m_) {
  auto m = dynamic_cast<messages::BlockRequest*> (m_);
  string key = m->block_name;
  peer.request(key, std::bind(&RemoteDFS::send_block, this, ph::_1, ph::_2));
}
// }}}
// request_ls {{{
void RemoteDFS::request_ls (messages::Message* m_) {
  auto m = dynamic_cast<messages::FileList*> (m_);
  peer.list(m);
  
  network->send(0, m);
}
// }}}
// send_block {{{
void RemoteDFS::send_block (std::string k, std::string v) {
  logger->info ("Sending Block %s", k.c_str());
  BlockInfo bi;
  bi.block_name = k;
  bi.content = v;

  network->send(0, &bi);
}
// }}}
// send_block {{{
void RemoteDFS::request_format (messages::Message* m_) {
  bool ret = peer.format();
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "File already exists";
  }

  network->send(0, &reply);
}
// }}}
