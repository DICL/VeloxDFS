#include "remotedfs.hh"
#include <functional>
#include <map>

using namespace eclipse;

RemoteDFS::RemoteDFS (Context& c) : Router(c), peer(c) {
  routing_table.insert({"BlockInfo", std::bind(&RemoteDFS::insert_block, this, 
        std::placeholders::_1)});
  routing_table.insert({"FileInfo", std::bind(&RemoteDFS::insert_file, this,
        std::placeholders::_1)});
}

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
