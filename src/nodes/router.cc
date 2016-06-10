#include "router.hh"
#include "../messages/factory.hh"

using namespace eclipse;
using namespace eclipse::messages;
using namespace std;

namespace eclipse {
// Constructor {{{
Router::Router(network::Network* net) : Node () {
  network = net;
  net->attach(this);
}

Router::~Router() { }
// }}}
// on_read {{{
void Router::on_read (Message* m, int n_channel) {
  string type = m->get_type();
  routing_table[type](m, n_channel);
}
// }}}
// on_disconnect {{{
void Router::on_disconnect (int id) {
}
// }}}
// on_connect() {{{
void Router::on_connect () {
  logger->info("Client connected to executor #%d", id);
}
// }}}
} /* eclipse  */
