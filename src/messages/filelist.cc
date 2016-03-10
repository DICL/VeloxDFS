#include "filelist.hh"

namespace eclipse {
namespace messages {

FileList::FileList (std::vector<FileInfo> v) : data(v) { }
std::string FileList::get_type() const { return "FileList"; }

}
}
