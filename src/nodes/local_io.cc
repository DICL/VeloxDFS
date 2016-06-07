#include "local_io.hh"
#include "../common/context_singleton.hh"
#include <cstdio>
#include <dirent.h>
#include <fstream>

using namespace eclipse;
using namespace std;

// constructors {{{
Local_io::Local_io() {
  disk_path = context.settings.get<string>("path.scratch");
}
//  }}}
// write {{{
void Local_io::write (std::string name, std::string v) {
  string file_path = disk_path + string("/") + name;
  ofstream file (file_path);
  file << v;
  file.close();
}
// }}}
// read {{{
std::string Local_io::read (string name) {
  ifstream in (disk_path + string("/") + name);
  string value ((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());

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
