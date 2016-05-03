#include "dfs.hh"
#include "../common/context.hh"

using namespace eclipse;

DFS::DFS(DIO* dio_): dio(dio_) {
  directory.init_db();
}
DFS::~DFS() { }
// insert_file {{{
bool DFS::insert_file (messages::File* f) {
 bool ret = directory.file_exist(f->file_name.c_str());

 if (ret) {
   INFO("File:%s exists in db, ret = %i", f->file_name.c_str(), ret);
   return false;
 }

 directory.insert_file_metadata(*f);

 INFO("Saving to SQLite db");
 return true;
}
// }}}
// insert_block {{{
bool DFS::insert_block (messages::Block* m) {
  string key = m->block_name;
  directory.insert_block_metadata(*m);
  dio->insert(m->block_hash_key, key, m->content);
  return true;
}
// }}}
// delete_block {{{
bool DFS::delete_block (messages::Block* m) {
  string file_name = m->file_name;
  unsigned int block_seq = m->block_seq;
  string key = m->block_name;
  dio->local_io.remove(key);
  directory.delete_block_metadata(file_name, block_seq);
  return true;
}
// }}}
// delete_file {{{
bool DFS::delete_file (std::string file_name) {
  bool ret = directory.file_exist(file_name.c_str());

  if (!ret) {
    INFO("File:%s doesn't exist in db, ret = %i", file_name.c_str(), ret);
    return false;
  }
 
  directory.delete_file_metadata(file_name);
 
  INFO("Removing from SQLite db");
  return true;
}
// }}}
// request_file {{{
FileDescription DFS::request_file (std::string file_name) {
  File fi;
  fi.num_block = 0;
  FileDescription fd;
  fd.file_name  = file_name;

  directory.select_file_metadata(file_name, &fi);

  int num_blocks = fi.num_block;
  for (int i = 0; i< num_blocks; i++) {
    Block bi;
    directory.select_block_metadata (file_name, i, &bi);
    string block_name = bi.block_name;
    fd.blocks.push_back(block_name);
    fd.hash_keys.push_back(bi.block_hash_key);
  }

  return fd;
}
// }}}
// list {{{
bool DFS::list (messages::List_files* m) {
  directory.select_all_file_metadata(m->data);
  return true;
}
// }}}
// format {{{
bool DFS::format () {
  INFO("Formating DFS");
  dio->local_io.format();
  directory.init_db();
  return true;
}
// }}}
// file_exist {{{
// FIXME need to think better name for this function
/**
 *@brief check we have given file name on our database
 *@param f is file name
 *@return return true if found that file on database otherwise return false
 */
bool DFS::file_exist (std::string file_name) {
  return directory.file_exist(file_name.c_str());
}
// }}}
// insert_key {{{
void DFS::insert_key (uint32_t key, std::string name, std::string value) {
  dio->insert_key(key, name, value);
}
// }}}
// request_key {{{
void DFS::request_key (std::string key, int origin) {
  dio->request_key(key, origin);
}
// }}}
// request {{{
void DFS::request (uint32_t key, string name , req_func f) {
  dio->request(key, name, f);
}
// }}}
