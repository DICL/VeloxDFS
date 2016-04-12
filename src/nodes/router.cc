#include "router.hh"
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
Router::Router() : Node () {
  port = context.settings.get<int>("network.port_mapreduce");
  network = new AsyncNetwork<Server> (this, port);
}

Router::~Router() { }
// }}}
// establish {{{
bool Router::establish () {
  network->establish();
  return true;
}
// }}}
// on_read {{{
void Router::on_read (Message* m) {
  string type = m->get_type();
  routing_table[type](m);
}
// }}}
// on_disconnect {{{
void Router::on_disconnect () {
  //network->on_disconnect(nullptr);
}
// }}}
// on_connect() {{{
void Router::on_connect () {
  logger->info("Client connected to executor #%d", id);
}
// }}}
} /* eclipse  */
