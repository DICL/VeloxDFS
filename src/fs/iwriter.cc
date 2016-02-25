#include "iwriter.h"
#include <string>
#include <map>
#include <iterator>
#include <fstream>
#include <utility>

using std::vector;
using std::multimap;
using std::string;

namespace eclipse {

IWriter::IWriter() {
  reduce_slot_ = con.settings.get<int>("mapreduce.reduce_slot");
  iblock_size_ = con.settings.get<int>("mapreduce.iblock_size");
  for (int i = 0; i < reduce_slot_; ++i) {
    kmv_blocks_.push_back(new multimap<string, string>());
    block_size_.push_back(0);
  }
  index_counter_ = 0;
}
IWriter::~IWriter() {
  for (int i = 0; i < reduce_slot_; ++i) {
    delete kmv_blocks_[i];
  }
}
void IWriter::AddKeyValue(const string &key, const string &value) {
  int index;
  index = round_robin(key);
  auto block = kmv_blocks_[index];

  int new_size;
  if (block->find(key) == block->end()) {
    new_size = get_block_size(index) + key.length() + value.length() + 2;
  } else {
    new_size = get_block_size(index) + value.length() + 1;
  }
  block->emplace(key, value);
  set_block_size(index, new_size);

  if (new_size > iblock_size_) {
    Flush(index);
  }
}
void IWriter::Flush(const int &index) {
  auto block = kmv_blocks_[index];
  std::ofstream file;
  string file_path;
  // TODO(wbkim): Get the path of intermediate data.
  // file_path = SomeFunctionToGetPath();
  file.open(file_path);

  // Write into the file.
  // TODO(wbkim): Should be changed into asynchronous manner.
  std::pair<multimap<string, string>::iterator,
      multimap<string, string>::iterator> ret;
  for (auto key_it = block->begin(); key_it != block->end(); key_it =
      ret.second) {
    ret = block->equal_range(key_it->first);
    file << key_it->first << std::endl;
    int num_value = std::distance(ret.first, ret.second);
    file << std::to_string(num_value) << std::endl;
    for (auto it = ret.first; it != ret.second; ++it) {
      file << it->second << std::endl;
    }
  }

  block->clear();
  set_block_size(index, 0);

  file.close();

  // TODO(wbkim): Write file information into db.
}
int IWriter::get_block_size(const int &index) {
  return block_size_[index];
}
void IWriter::set_block_size(const int &index, const int &size) {
  block_size_[index] = size;
}
int IWriter::round_robin(const string &key) {
  auto it = key_index_.find(key);
  if (it != key_index_.end()) {
    return it->second;
  } else {
    int index = index_counter_++ % reduce_slot_;
    key_index_.emplace(key, index);
    return index;
  }
}

}  // namespace eclipse
