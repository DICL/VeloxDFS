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
  routing_table.insert({"BlockRequest", bind(&RemoteDFS::request_file, this, _1)});
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
// }}}
// request_file {{{
void RemoteDFS::request_file (messages::Message* m_) {
  auto m = dynamic_cast<messages::FileRequest*> (m_);
  logger->info ("FileInfo received");

  auto fd = peer.request_file (m);
  network->send(0, &fd);
}
// }}}
// send_block {{{
void RemoteDFS::request_block (messages::Message* m ) {
 //   
}
// }}}
