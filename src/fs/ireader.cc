#include "ireader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using std::ifstream;
using std::string;

namespace eclipse {

IReader::IReader() {
  // TODO(wbkim): Load the number of iblocks from db.
  // num_block_ = GetNumBlock();
  num_finished_ = 0;
  is_next_key_ = true;
  is_next_value_ = true;
  is_clear_ = true;
  num_remain_.reserve(num_block_);
  loaded_keys_.reserve(num_block_);
  for (int i = 0; i < num_block_; ++i) {
    // TODO(wbkim): Load the block information from db.
    // blocks_.push_back(new ifstream(block_name));
    LoadKey(i);
  }
  SetNext();
}
IReader::~IReader() {
}
bool IReader::is_next_key() {
  return is_next_key_;
}
bool IReader::is_next_value() {
  return is_next_value_;
}
bool IReader::get_next_key(string *key) {
  if (!is_next_key_) return false;
  if (!is_clear_) {
    if (!ShiftToNextKey()) return false;
    is_clear_ = false;
  }
  SetNextAsCurrent();
  *key = curr_key_;
  LoadValue(curr_block_index_);
  is_next_value_ = true;
  return true;
}
bool IReader::get_next_value(string *value) {
  *value = next_value_;
  if (num_remain_[curr_block_index_] > 0) {
    LoadValue(curr_block_index_);
  } else {  // All the values of current key from current file has been read.
    if (!LoadKey(curr_block_index_)) {  // File ends.
      if (!FinishBlock(curr_block_index_)) return false;
    }
    SetNext();
    if (next_key_ == curr_key_) {  // Same key exist on another file.
      SetNextAsCurrent();
      LoadValue(curr_block_index_);
    } else {  // All the values of current key from whole files has been read.
      is_next_value_ = false;
      is_clear_ = true;
    }
  }
  return true;
}
void IReader::SetNext() {
  next_it_ = get_min_iterator();
  next_block_index_ = next_it_->second;
  next_key_ = next_it_->first;
  is_next_key_ = true;
}
void IReader::SetNextAsCurrent() {
  key_order_.erase(next_it_);
  curr_block_index_ = next_block_index_;
  curr_key_ = next_key_;
}
bool IReader::ShiftToNextKey() {
  for (int i = 0; i < num_block_; ++i) {
    if (loaded_keys_[i] == curr_key_) {
      ifstream *block = blocks_[i];
      int pos = block->tellg();
      int num_remain = num_remain_[i];
      block->seekg(pos + num_remain);
      if (!LoadKey(i)) {  // File ends.
        if (!FinishBlock(i)) return false;
      }
    }
  }
  return true;
}
bool IReader::LoadKey(const int &index) {
  // Make sure you are not in the middle of the values.
  if (blocks_[index]->eof()) return false;
  getline(*blocks_[index], loaded_keys_[index]);
  string num_value;
  getline(*blocks_[index], num_value);
  num_remain_[index] = stoi(num_value);
  key_order_.emplace(loaded_keys_[index], index);
  return true;
}
bool IReader::LoadValue(const int &index) {
  getline(*blocks_[index], next_value_);
  --num_remain_[curr_block_index_];
}
std::multimap<string, int>::iterator IReader::get_min_iterator() {
  return key_order_.begin();
}
bool IReader::FinishBlock(const int &index) {
  blocks_[index]->close();
  delete blocks_[index];
  loaded_keys_[index] = "";
  ++num_finished_;
  if (num_finished_ == num_block_) {
    is_next_key_ = false;
    return false;
  }
  return true;
}

}  // namespace eclipse
