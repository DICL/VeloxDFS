#include "blockdel.hh"
#include "reply.hh"

using namespace eclipse::messages;

std::string BlockDel::get_type() const { return "BlockDel"; }

void BlockDel::exec(PeerDFS* p, message_fun f) {

  bool ret = p->delete_block(&block);
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "OK";
  } else {
    reply->message = "FAIL";
    reply->details = "Block doesn't exist";
  }
  f(reply);
}
