#include "filelist.hh"

using namespace eclipse::messages;

FileList::FileList() {}

FileList::FileList(std::vector<FileInfo> data) {
  this->data.assign(data.begin(), data.end());
}

std::string FileList::get_type() const { return "FileList"; }
