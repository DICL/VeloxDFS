#ifndef SRC_FS_IWRITER_H_
#define SRC_FS_IWRITER_H_
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include "../common/context.hh"

using std::vector;
using std::multimap;
using std::unordered_map;
using std::string;

namespace eclipse {

class IWriter {
 public:
  IWriter();
  ~IWriter();

  void AddKeyValue(const string &key, const string &value);

 private:
  void Flush(const int &index);
  int get_block_size(const int &index);
  void set_block_size(const int &index, const int &size);
  int round_robin(const string &key);

  Context con;
  int reduce_slot_;
  int iblock_size_;
  int index_counter_;
  vector<multimap<string, string>*> kmv_blocks_;
  vector<int> block_size_;
  unordered_map<string, int> key_index_;
};

}  // namespace eclipse
#endif  // SRC_FS_IWRITER_H_
