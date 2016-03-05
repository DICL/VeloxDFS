#include "executor.hh"
#include "../common/dl_loader.hh"
#include "../network/asyncnetwork.hh"
#include "../network/server.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"

#include <string>
#include <sstream>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace std;

namespace eclipse {
// Constructor {{{
Executor::Executor(Context& context) : Node (context), peer_cache (context) {

  port = context.settings.get<int>("network.port_mapreduce");
  network = new AsyncNetwork<Server> (this, context, 1, port);
}

Executor::~Executor() { }
// }}}
// establish {{{
bool Executor::establish () {
  peer_cache.establish();
  network->establish();
  return true;
}
// }}}
// run_map {{{
void Executor::run_map (messages::Task* m, std::string input) {
    DL_loader loader (m->library);

    try {
      loader.init_lib();
    } catch (std::exception& e) {
      logger->error ("Not found library path[%s]", m->library.c_str());
    }

    auto _map_ = loader.load_function(m->func_name);
    stringstream ss (input);

    while (ss.eof()) {
      char next_line [256];
      ss.getline (next_line, 256);
      auto key_value = _map_ (string(next_line));

      auto& key   = key_value.first;
      auto& value = key_value.second;
      peer_cache.insert (key, value);
    }
  }
// }}}
// process (Task* m) {{{
template<> void Executor::process (messages::Task* m) {
  if (m->type == MAP) {
    peer_cache.request(m->input_path, std::bind(
          &Executor::run_map, this, m, 
          std::placeholders::_1));
  }
}
// }}}
// process (FileInfo* m) {{{
template<> void Executor::process (messages::FileInfo* m) {
  logger->info ("FileInfo received");
  bool ret = peer_cache.store(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "File already exists";
  }

  network->send(0, &reply);
}
// }}}
// process (BlockInfo* m) {{{
template<> void Executor::process (messages::BlockInfo* m) {
  logger->info ("BlockInfo received");
  bool ret = peer_cache.insert_block(m);
  Reply reply;

  if (ret) {
    reply.message = "OK";

  } else {
    reply.message = "FAIL";
    reply.details = "Block already exists";
  }

  network->send(0, &reply);
}
// }}}
// process (Control* m) {{{
template<> void Executor::process (Control* m) {
  switch (m->type) {
    case SHUTDOWN:
      exit(EXIT_SUCCESS);
      break;

    case RESTART:
      break;
  }
}
// }}}
// process (CacheInfo* m) {{{
template<> void Executor::process (CacheInfo* m) {
  vec_str out = peer_cache.info();
  m->keys = out;
  network->send(0, m);
}
// }}}
// on_read {{{
void Executor::on_read (Message* m) {
  string type = m->get_type();

  if (type == "Task") {
    auto m_ = dynamic_cast<Task*>(m);
    process(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process(m_);

  } else if (type == "FileInfo") {
    auto m_ = dynamic_cast<FileInfo*>(m);
    process(m_);

  } else if (type == "BlockInfo") {
    auto m_ = dynamic_cast<BlockInfo*>(m);
    process(m_);

  } else if (type == "CacheInfo") {
    auto m_ = dynamic_cast<CacheInfo*>(m);
    process(m_);

  } else {
    logger->error ("Messaged not managed received in Executor");
  }
}
// }}}
// on_disconnect {{{
void Executor::on_disconnect () {
  network->on_disconnect();
}
// }}}
// on_connect() {{{
void Executor::on_connect () {
  logger->info("Client connected to executor #%d", id);
}
// }}}
} /* eclipse  */
