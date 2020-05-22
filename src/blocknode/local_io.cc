#include "local_io.hh"
#include "../common/context_singleton.hh"
#include <cstdio>
#include <dirent.h>
#include <vector>
#include <algorithm>
#include <ftw.h>

#include <fcntl.h>
#include <ext/stdio_filebuf.h>
#include <sstream>


using namespace eclipse;
using namespace std;

uint64_t BLOCK_SIZE = context.settings.get<int>("filesystem.block");
// constructors {{{
Local_io::Local_io() {
  disk_path = context.settings.get<string>("path.scratch");
}
//  }}}
// write {{{
//! @brief Unbuffered write to disk
void Local_io::write (const std::string& name, const std::string& v) {
	//split path  
  std::size_t curpos = name.find_first_not_of("/", 0);
  std::size_t delimpos = name.find_first_of("/", curpos);

	std::vector<std::string> path_tokens;

  while(true){
    if(curpos == std::string::npos) break;
    path_tokens.push_back(name.substr(curpos, delimpos-curpos));
    curpos = name.find_first_not_of("/", delimpos);
    delimpos = name.find_first_of("/", curpos);
  }
	//

  ofstream file;
  string curPath = disk_path;

	//make dir if not exist
  for(string& token : path_tokens){
    curPath += "/" + token;
    
    if(token == path_tokens.back()){
      file.rdbuf()->pubsetbuf(0, 0);      //! No buffer
      file.open(curPath, ios::binary);  //! Binary write
  	  file.write(v.c_str(), v.length());
      file.close();
    }
    else {
      struct stat st = {0};
      //create directory
      if(stat(curPath.c_str(), &st) == -1){
        mkdir(curPath.c_str(), 0777);
      }
    }
  }
//
}
// }}}
// update {{{
//void Local_io::update (const std::string& name, const std::string& v, uint32_t pos, uint32_t len) {
void Local_io::update (const std::string& name, const std::string& v, uint64_t pos, uint64_t len) {
  string file_path = disk_path + string("/") + name;
  fstream file (file_path, fstream::binary | fstream::in | fstream::out);
	/*if( !file ){
	cout << "File is not existed" << endl;
		ofstream make_file(file_path, ostream::binary | ostream::out);

		make_file.close();
		file.open(file_path, fstream::binary | fstream::in | fstream::out);
	}*/
  file.seekp(pos, ios_base::beg);
  file.write(v.c_str(), len);
  file.close();
}
// }}}
// append {{{
void Local_io::append (const std::string& name, const std::string& v, uint64_t len) {
  string file_path = disk_path + string("/") + name;
  fstream file (file_path, fstream::binary | fstream::out | fstream::app | fstream::ate);
  if(!file){
    cout << "File is not existed" << endl;
	exit(0);
  }
  file.write(v.c_str(), len);
  file.close();
  //FILE* fp = fopen(file_path, "ab");
  //fwrite(&v.c_str[0], sizeof(char), len, fp);
  //fsync(fileno(fp));
  //fclose(fp);
}
// }}}
// read {{{
std::string Local_io::read (const string& name) {
  return read(name, 0, 0, true);
}
//std::string Local_io::read (const string& name, uint32_t off, uint32_t len) {
std::string Local_io::read (const string& name, uint64_t off, uint64_t len) {
  return read(name, off, len, false);
}

//std::string Local_io::read (const string& name, uint32_t off, uint32_t len, bool is_whole = false) {
std::string Local_io::read (const string& name, uint64_t off, uint64_t len, bool is_whole = false) {
  ifstream in (disk_path + string("/") + name, ios::in | ios::binary | ios::ate);
  uint64_t file_size = (uint64_t)in.tellg();
  in.seekg(off, ios::beg);

  if(is_whole) 
    len = file_size;

  uint64_t readable_len = std::min(len, (file_size - off));

  vector<char> bytes(readable_len);
  in.read(&bytes[0], readable_len);

  in.close();
  return string(&bytes[0], readable_len);
}

std::string Local_io::batch_read(const string& name, uint32_t totalLen, vector< pair<uint64_t, uint64_t > > &block_info ){
	ifstream in (disk_path + string("/") + name, ios::in | ios::binary | ios::ate);

  vector<char> bytes(totalLen);
	for(int i = 0; i < (int)block_info.size() ; i++){
  	  in.seekg(block_info[i].first, ios::beg);
  	  in.read(&bytes[0]+block_info[i].second, block_info[i].second);
	}
  in.close();
  return string(&bytes[0], totalLen);
}
// }}}
// read_metadata {{{
// This functions loads into a string the metadata db
std::string Local_io::read_metadata() {
  string replica_path = context.settings.get<string>("path.metadata") + "/metadata.db";
  ifstream in (replica_path);
  string value ((std::istreambuf_iterator<char>(in)),
      std::istreambuf_iterator<char>());

  return value;
}
// }}}
// format {{{
bool Local_io::format () {
  string fs_path = context.settings.get<string>("path.scratch");
  string md_path = context.settings.get<string>("path.metadata");

  // Make me more elegant! 
  int ret = nftw(fs_path.c_str(), 
      [] (const char* path, const struct stat*, int, struct FTW*) -> int {
        int ret = 0;

        string fs_path = GET_STR("path.scratch");
        if (fs_path != string(path)) {
          DEBUG("FORMAT: Removing %s", path);

          if (0 != (ret = ::remove(path))) {
            ERROR("FORMAT: Can't remove %s.", path);
          }
         }
        return ret;
      }, 10, FTW_DEPTH);

  if (ret != 0) {
    ERROR("FORMAT: Error formating data space");
  }

  ::remove((md_path + "/metadata.db").c_str());
  return true;
}
// }}}
// remove {{{
void Local_io::remove (const std::string& k) {
  string file_path = disk_path + string("/") + k;
  ::remove(file_path.c_str());
}


// }}}

