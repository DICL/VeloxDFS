#include "formatrequest.hh"
#include "reply.hh"

using namespace eclipse::messages;

std::string FormatRequest::get_type() const { return "FormatRequest"; }

void FormatRequest::exec(FS* p, message_fun f) {
  bool ret = p->format();
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "OK";

  } else {
    reply->message = "FAIL";
    reply->details = "File already exists";
  }
  f(reply);

}
