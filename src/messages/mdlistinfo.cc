#include "mdlistinfo.hh"

using namespace eclipse::messages;

MdlistInfo::MdlistInfo() {}

MdlistInfo::MdlistInfo(std::string file_name, std::string slice_name, uint64_t file_pos,
    uint64_t slice_pos, uint64_t len) {
  this->file_name = file_name;
  this->slice_name = slice_name;
  this->file_pos = file_pos;
  this->slice_pos = slice_pos;
  this->len = len;
}

std::string MdlistInfo::get_type() const { return "MdlistInfo"; }
