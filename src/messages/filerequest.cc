#include "filerequest.hh"

using namespace eclipse::messages;

FileRequest::FileRequest() {}

FileRequest::FileRequest(std::string name) {
  this->name = name;
}

std::string FileRequest::get_type() const { return "FileRequest"; }
