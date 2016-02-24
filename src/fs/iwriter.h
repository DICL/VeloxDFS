#ifndef SRC_FS_IWRITER_H_
#define SRC_FS_IWRITER_H_
#include <string>
#include <map>
#include <vector>
#include <list>
#include "../common/context.hh"

using std::vector;
using std::multimap;
using std::string;

namespace eclipse {

class IWriter {
 public:
  IWriter();
  ~IWriter();

  void AddKeyValue(string key, string value);

 private:
  void Flush(int index);
  int get_block_size(int index);
  void set_block_size(int index, int size);

  Context con;
  int reduce_slot_;
  int iblock_size_;
  vector<multimap<string, string>*> kmv_blocks_;
  vector<int> block_size_;
};

}  // namespace eclipse
#endif  // SRC_FS_IWRITER_H_
