#include "local_io.hh"
#include "../common/context_singleton.hh"
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <vector>

using namespace eclipse;
using namespace std;

// constructors {{{
Local_io::Local_io() {
}
//  }}}
// write {{{
void Local_io::write(string name, string& v) {
  string disk_path = context.settings.get<string>("path.scratch");
  string file_path = disk_path + "/" + name;
  ofstream file (file_path);
  file << v;
  file.close();
}
// }}}
// update {{{
void Local_io::update(string name, string v, uint64_t pos, uint64_t len) {
  string disk_path = context.settings.get<string>("path.scratch");
  string file_path = disk_path + string("/") + name;
  fstream file (file_path, ios::out | ios::binary);
  file.seekp(pos, ios::beg);
  file.write(v.c_str(), len);
  file.close();
}
// }}}
// read {{{
string Local_io::read(string name) {
  string disk_path = context.settings.get<string>("path.scratch");
  ifstream in (disk_path + string("/") + name, ios::in | ios::binary | ios::ate);
  ifstream::pos_type fileSize = in.tellg();
  in.seekg(0, ios::beg);
  vector<char> bytes(fileSize);
  in.read(&bytes[0], fileSize);
  in.close();
  return string(&bytes[0], fileSize);
}
// }}}
// read_metadata {{{
// This functions loads into a string the metadata db
string Local_io::read_metadata() {
  string md_path = context.settings.get<string>("path.metadata") + "/metadata.db";
  ifstream in(md_path);
  string value((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
  in.close();
  return value;
}
// }}}
// pread {{{
string Local_io::pread(string name, uint64_t pos, uint64_t len) {
  string disk_path = context.settings.get<string>("path.scratch");
  ifstream in(disk_path + string("/") + name);
  in.seekg(pos, in.beg);
  char *buffer = new char[len];
  in.read(buffer, len);
  string value(buffer);
  delete[] buffer;
  in.close();
  return value;
}
// }}}
// format {{{
bool Local_io::format () {
  string disk_path = context.settings.get<string>("path.scratch");
  string md_path = context.settings.get<string>("path.scratch");
  DIR *theDir = opendir(disk_path.c_str());
  char buf[512] = {0};
  sprintf(buf, "%s", md_path.c_str());
  std::remove(buf);
  if (theDir) {
    struct dirent *next_file;
    while ((next_file = readdir(theDir)) != NULL) {
      if (!strcmp(next_file->d_name, ".") || !strcmp(next_file->d_name, "..")) {
        continue;
      }
      sprintf(buf, "%s/%s", disk_path.c_str(), next_file->d_name);
      std::remove(buf);
    }
  }
  closedir(theDir);
  return true;
}
// }}}
// remove {{{
void Local_io::remove (string k) {
  string disk_path = context.settings.get<string>("path.scratch");
  string file_path = disk_path + string("/") + k;
  std::remove(file_path.c_str());
}
// }}}
