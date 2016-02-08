#include "executor.hh"
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
Executor::Executor(Context& context) : MR_traits (context), peer_cache (context) {
  peer_remote = new PeerRemote(this, 0);
}

Executor::~Executor() { }
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
  if (m->type == 0) {
    peer_cache.request(m->input_path, std::bind(&Executor::run_map,         this, m, std::placeholders::_1));
  }
}
// }}}
// process (Control* m) {{{
template<> void Executor::process (Control* m) {
  switch (m->type) {
    case messages::SHUTDOWN:
      exit(EXIT_SUCCESS);
      break;

    case messages::RESTART:
      break;

      //    case PING:
      //      process_ping (m);
      //      break;
  }
}
// }}}
// process_message (Message*) {{{
void Executor::process_message (Message* m) {
  string type = m->get_type();

  if (type == "Task") {
    auto m_ = dynamic_cast<messages::Task*>(m);
    process(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process(m_);
  }
}
// }}}
// action {{{
void Executor::action (tcp::socket* sock) {
  Channel* c = new Channel (sock);
  c->set_recv_socket(sock);
  peer_remote->set_channel(c);
}
// }}}

} /* eclipse  */
