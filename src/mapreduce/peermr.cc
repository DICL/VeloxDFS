#include "peermr.hh"

namespace eclipse {
// Constructor{{{
PeerMR::PeerMR (Context& c) : Peer (c) {
  directory.init_db();
}

PeerMR::~PeerMR () {

}
// }}} 
// insert {{{
bool PeerMR::insert (std::string key, std::string val) {
  if (exists(key)) {
    auto value = cache->get(key);
    value += string("\n") + val;
    cache->put(key, value);

  } else {
    insert(key, val);
  }

  return true;
}
// }}}
// lookup {{{
void PeerMR::lookup (std::string key, req_func f) {
 if (exists(key)) {
   string value = cache->get(key);
   f(value);
 
 } else {
   if (belongs(key)) {
     //Read from disk
   } else {
     request(key, f);
   }
 }
}
// }}}
// store {{{
bool PeerMR::store (messages::FileInfo* f) {
 //Save it to sqlite db
 bool ret = directory.is_exist(f->file_name.c_str());

 if (ret) {
   logger->info ("File:%s exists in db, ret = %i", f->file_name.c_str(), ret);
   return false;
 }

 directory.insert_file_metadata(*f);

 logger->info ("Saving to SQLite db");
 return true;
}
// }}}
// insert_block {{{
bool PeerMR::insert_block (messages::BlockInfo* m) {
  string key = m->block_name;
  int which_node = H(key);

  directory.insert_block_metadata(*m);

  Peer::insert(key, m->content);

  return true;
}
// }}}
} /* eclipse  */ 
