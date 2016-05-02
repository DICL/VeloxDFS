#include "filelist.hh"

namespace eclipse {
namespace messages {

//FileList::FileList (std::vector<FileInfo> v) : data(v) { }
std::string FileList::get_type() const { return "FileList"; }


void FileList::exec(PeerDFS* p, message_fun f) {
  p->list(&list_of_files);
  f(this);
}

}
}
