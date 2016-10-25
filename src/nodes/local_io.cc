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
  disk_path = context.settings.get<string>("path.scratch");
}
//  }}}
// write {{{
void Local_io::write (std::string name, std::string& v) {
  string file_path = disk_path + string("/") + name;
  ofstream file (file_path);
  file << v;
  file.close();
}
// }}}
// update {{{
void Local_io::update (std::string name, std::string v, uint32_t pos, uint32_t len) {
  string file_path = disk_path + string("/") + name;
  fstream file (file_path, fstream::binary | fstream::in | fstream::out);
  file.seekp(pos, ios_base::beg);
  file.write(v.c_str(), len);
  file.close();
}
// }}}
// read {{{
std::string Local_io::read (string name) {
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
std::string Local_io::read_metadata() {
  string replica_path = context.settings.get<string>("path.metadata") + "/metadata.db";
  ifstream in (replica_path);
  string value ((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());

  in.close();
  return value;
}
// }}}
// pread {{{
std::string Local_io::pread (string name, uint32_t pos, uint32_t len) {
  ifstream in (disk_path + string("/") + name);
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
  string fs_path = context.settings.get<string>("path.scratch");
  string md_path = context.settings.get<string>("path.metadata");

  DIR *theFolder = opendir(fs_path.c_str());
  struct dirent *next_file;
  char filepath[256] = {0};

  while ( (next_file = readdir(theFolder)) != NULL ) {
    sprintf(filepath, "%s/%s", fs_path.c_str(), next_file->d_name);
    DEBUG("FORMAT: Removing %s", filepath);
    if (0 != ::remove(filepath)) {
      ERROR("FORMAT: Can't remove %s.", filepath);
    }
  }
  closedir(theFolder);

  ::remove((md_path + "/metadata.db").c_str());
  return true;
}
// }}}
// remove {{{
void Local_io::remove (std::string k) {
  string file_path = disk_path + string("/") + k;
  ::remove(file_path.c_str());
}
// }}}
