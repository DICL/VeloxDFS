#ifndef SRC_FS_IREADER_H_
#define SRC_FS_IREADER_H_
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include "../common/context.hh"

using std::vector;
using std::ifstream;
using std::string;
using std::multimap;

namespace eclipse {

class IReader {
 public:
  IReader();
  ~IReader();

  bool get_next_key(string *key);
  bool get_next_value(string *value);
  bool is_next_key();
  bool is_next_value();

 private:
  void SetNext();
  void SetNextAsCurrent();
  bool ShiftToNextKey();
  bool LoadKey(const int &index);
  bool LoadValue(const int &index);
  bool FinishBlock(const int &index);

  multimap<string, int>::iterator get_min_iterator();

  Context con;
  int num_block_;
  int num_finished_;
  bool is_next_key_;
  bool is_next_value_;
  bool is_clear_;
  vector<ifstream*> blocks_;
  vector<string> loaded_keys_;
  vector<int> num_remain_;
  int next_block_index_;
  int curr_block_index_;
  string curr_key_;
  string next_key_;
  string next_value_;
  multimap<string, int> key_order_;
  multimap<string, int>::iterator next_it_;
};

}  // namespace eclipse
#endif  // SRC_FS_IREADER_H_
