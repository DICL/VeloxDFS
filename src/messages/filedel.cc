#include "filedel.hh"
#include "reply.hh"

namespace eclipse {
namespace messages {
std::string FileDel::get_type() const { return "FileDel"; }

void FileDel::exec(PeerDFS* p, message_fun f) {
  bool ret = p->delete_file(file_name);
  auto* reply = new Reply();

  if (ret)
    reply->message = "OK";
  else 
    reply->message = "FAIL";

  f(reply);
}
}
}
