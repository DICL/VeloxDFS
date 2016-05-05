// includes & usings {{{
#include "dio.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"
#include "../messages/keyrequest.hh"

#include <algorithm>
#include <iterator>
#include <memory>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace std;

// }}}

namespace eclipse {
// Constructor & destructor {{{
DIO::DIO (Network* net) : Node () { 
  network = net;

  int size = context.settings.get<vec_str>("network.nodes").size();
  boundaries.reset( new Histogram {size, 0});
  boundaries->initialize();
}

DIO::~DIO() { }
// }}}
// insert {{{
void DIO::insert (uint32_t hash_key, std::string name, std::string v) {
  int which_node = boundaries->get_index(hash_key);

  if (which_node == id) {
    INFO("[DFS] Saving locally KEY: %s", name.c_str());
    local_io.write(name, v);

  } else {
    INFO("[DFS] Forwaring KEY: %s -> %d", name.c_str(), which_node);
    KeyValue kv (hash_key, name, v);
    network->send(which_node, &kv);
  }
}
// }}}
// request {{{
void DIO::request (uint32_t key, string name , req_func f) {
 int idx = boundaries->get_index(key);

 if (idx != id) {
   KeyRequest k_req (name);
   k_req.set_origin (id);
   network->send (idx, &k_req);
   requested_blocks.insert ({name, f});

 } else {
  string value = local_io.read(name);
  f(name, value);
 }
}
// }}}
// close {{{
void DIO::close() { exit(EXIT_SUCCESS); }
// }}}
// insert_key {{{
void DIO::insert_key (uint32_t key, std::string name, std::string value) {
  int which_node = boundaries->get_index(key);
  if (which_node == id)  {
    logger->info ("Instering key = %s", name.c_str());
    insert(key, name, value);
  }

  if (requested_blocks.find(name) != requested_blocks.end()){
    logger->info ("Executing func");
    requested_blocks[name](name, value);
    requested_blocks.erase(name);
  }
}
// }}}
// request_key {{{
void DIO::request_key (std::string key, int origin) {
  INFO("Arrived req key = %s", key.c_str());
  string value = local_io.read(key);

  KeyValue kv (0, key, value);
  kv.destination = origin;
  network->send(origin, &kv);
}
// }}}
}
