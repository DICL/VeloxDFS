#include "iwriter.hh"
#include "fileio.hh"
#include <string>
#include <unordered_map>

#define IBLOCKSIZE 64*1024*1024

using namespace eclipse;
using namespace std;

IWriter::IWriter() {
  const int REDUCE_SLOT = 8;
  for (int i = 0; i < REDUCE_SLOT; ++i) {
    kmv_blocks_.push_back(new unordered_multimap<string, string>());
    block_size_.push_back(0);
  }
}
IWriter::~IWriter() {
  const int REDUCE_SLOT = 8;
  for (int i = 0; i < REDUCE_SLOT; ++i) {
    delete kmv_blocks.at(i);
  }
}
void IWriter::AddKeyValue(string key, string value) {
  int index;
  //index = SomeHashFunction(key);
  auto block = kmv_blocks_.at(index);

  int new_size;
  if (block->find(key) == block->end) {
    new_size = get_block_size() + key.length() + value.length() + 2;
  } else {
    new_size = get_block_size() + value.length() + 1;
  }
  block->emplace(key, value);
  set_block_size(index, new_size);

  if (new_size > IBLOCKSIZE) {
    Flush(index);
  }
}
void IWriter::Flush(int index) {
  auto block = kmv_blocks_.at(index);
  FileIO file;
  string file_path;
  //file_path = SomeFunctionToGetPath();
  file.open_wfile(file_path);

  pair<unorderd_multimap<string, string>::iterator,
    unordered_multimap<string, string>::iterator> ret;
  for (auto key_it = block->begin(); key_it != block->end(); key_it =
    ret.second) {
    ret = block->equal_range(key_it->first);
    for (auto it = ret.first; it != ret.second; ++it) {
      // Do something to wirte file
    }
  }

  block->clear();
  set_block_size(index, 0);

  file.close_file();
}
int IWriter::get_block_size(int index) {
  return block_size_[index];
}
void Iwriter::set_block_size(int index, int size) {
  block_size_.at(index) = size;
}
