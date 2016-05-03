#include "blockinfo.hh"
#include "reply.hh"

using namespace eclipse::messages;

std::string BlockInfo::get_type() const { return "BlockInfo"; }

void BlockInfo::exec (FS* p, message_fun f) {
  bool ret = p->insert_block(&block);
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "OK";

  } else {
    reply->message = "FAIL";
    reply->details = "Block already exists";
  }

  f(reply);
}
