#include <iostream>
#include <cstring>

#include <eclipsedfs/vdfs.hh>

using namespace velox;
using namespace std;

std::ostream& operator<<(std::ostream& os, const velox::model::metadata& md) {
  os << "------------------------------------------" << std::endl;
  os << "|name\t\t|hash_key\t|size\t|num_block\t|type\t|replica\t|"<< std::endl;
  os << "|" << md.name << "\t|" << md.hash_key << "\t|" << md.size << "\t|" << md.num_block << "\t\t|" << md.type << "\t|" << md.replica << "\t\t|"<< std::endl;
  return os;
}

int main () {
  vdfs cloud;
  long fd = cloud.open_file("test.txt");
  cout << "test.txt is opened." << endl;

  int write_length = 13;
  char input[write_length];
  char abc[3] = { 'a', 'b', 'c' };

  //for(int i=0; i<3; i++) {
  //  model::metadata md = cloud.get_metadata(fd);

  //  memset(input, abc[i], 3);
  //  cloud.write(fd, input, md.size, write_length);
  //}

  model::metadata md = cloud.get_metadata(fd);

  strncpy(input, abc, 3);
  cloud.write(fd, input, md.size, 3);
  cloud.append("test.txt", " THIS LINE GOES AT LAST");

  cout << "write" << endl;

  int read_length = 3;
  char str[read_length];
  cloud.read(fd, str, 0, read_length);

  cout << "read: " << str << endl;

  DFS dfs;
  cout << "read all: " << dfs.read_all("test.txt") << endl;

  md = cloud.get_metadata(fd);

  cout << md << endl;

  cloud.close(fd);

  cout << "close" << endl;

  cloud.rm("test.txt");



  return 0;
}
