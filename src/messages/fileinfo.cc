#include "fileinfo.hh"
#include "reply.hh"

using namespace eclipse::messages;

std::string FileInfo::get_type() const { return "FileInfo"; }

void FileInfo::exec(FS* p, message_fun f) {
  bool ret = p->insert_file (&file);
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "OK";

  } else {
    reply->message = "FAIL";
  }

  f(reply);
}
