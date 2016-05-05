#include "fileexist.hh"
#include "reply.hh"

using namespace eclipse::messages;

std::string FileExist::get_type() const { return "FileExist"; }

void FileExist::exec(FS* p, message_fun f) {
  bool ret = p->file_exist(file_name);
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "TRUE";

  } else {
    reply->message = "FALSE";
  }
  f(reply);
}
