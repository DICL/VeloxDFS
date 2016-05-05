#include "filerequest.hh"

namespace eclipse {
namespace messages {

std::string FileRequest::get_type() const { return "FileRequest"; }

void FileRequest::exec(FS* p, message_fun f) {
  auto fd = p->request_file (file_name);
  f(&fd);
}

}
}
