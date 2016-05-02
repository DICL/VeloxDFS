#include "router.hh"
#include "../messages/executable.hh"
#include "../messages/factory.hh"

using namespace eclipse;
using namespace eclipse::messages;
using namespace std;
namespace ph = std::placeholders;

namespace eclipse {
// Constructor {{{
Router::Router(network::Network* net, PeerDFS* p) : Node () {
  network = net;
  net->attach(this);
  peer = p;
}

Router::~Router() { }
// }}}
// on_read {{{
void Router::on_read (Message* m, int n_channel) {
  if(dynamic_cast<Executable<PeerDFS>*>(m)) {
    on_read_peerdfs(m, n_channel);
  }
}
// }}}
// on_read_peerdfs {{{
void Router::on_read_peerdfs (Message* m, int n_channel) {
  auto ex = dynamic_cast<Executable<PeerDFS>*>(m);
  ex->exec(peer, std::bind(&Router::async_reply, this, ph::_1, n_channel));
}
// }}}
// on_disconnect {{{
void Router::on_disconnect (int id) {
}
// }}}
// on_connect() {{{
void Router::on_connect () {
  INFO("Client connected to executor #%d", id);
}
// }}}
// async_reply {{{
void Router::async_reply (messages::Message* m, int n_channel) {
  network->send(n_channel, m);
}
// }}}
} /* eclipse  */
