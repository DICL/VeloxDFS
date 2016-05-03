#include "fileinfo.hh"
#include "reply.hh"

namespace eclipse {
namespace messages {
std::string FileInfo::get_type() const { return "FileInfo"; }

void FileInfo::exec(FS* p, message_fun f) {
  bool ret = p->file_exist(file.file_name);
  Reply* reply = new Reply();

  if (ret) {
    reply->message = "TRUE";

  } else {
    reply->message = "FALSE";
  }

  f(reply);
}

}
}
