#include "local_io.hh"
#include "../common/context_singleton.hh"
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace eclipse;
using namespace std;

// constructors {{{
Local_io::Local_io() {
  disk_path = context.settings.get<string>("path.scratch");
}
//  }}}
// write {{{
//! @brief Unbuffered write to disk
void Local_io::write (std::string name, std::string& v) {
  string file_path = disk_path + string("/") + name;
  ofstream file;

  file.rdbuf()->pubsetbuf(0, 0);      //! No buffer
  file.open(file_path, ios::binary);  //! Binary write
  file.write(v.c_str(), v.length());
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
  return read(name, 0, 0, true);
}

std::string Local_io::read (string name, uint32_t off, uint32_t len) {
  return read(name, off, len, false);
}

std::string Local_io::read (string name, uint32_t off, uint32_t len, bool is_whole = false) {
  ifstream in (disk_path + string("/") + name, ios::in | ios::binary | ios::ate);
  uint32_t file_size = (uint32_t)in.tellg();
  in.seekg(off, ios::beg);

  if(is_whole) 
    len = file_size;

  uint32_t readable_len = std::min(len, (file_size - off));

  vector<char> bytes(readable_len);
  in.read(&bytes[0], readable_len);

  in.close();
  return string(&bytes[0], readable_len);
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
    if (strncmp(basename(filepath), "..", 256) == 0 or
        strncmp(basename(filepath), "...", 256) == 0 or
        strncmp(basename(filepath), ".", 256) == 0)
      continue;

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
