#include <nodes/remotedfs.hh>
#include <common/context_singleton.hh>
#include <network/p2p.hh>
#include <network/server.hh>
#include <network/asyncnetwork.hh>
#include <nodes/router.hh>
#include <string>

using namespace eclipse;
using namespace eclipse::network;

int main (int argc, char ** argv) {
  int in_port = context.settings.get<int>("network.ports.internal");
  int ex_port = context.settings.get<int>("network.ports.client");

  network::Network* internal_net = new network::AsyncNetwork<P2P>(in_port);
  PeerDFS peer (internal_net);
  internal_net->establish();

  network::Network* external_net = new network::AsyncNetwork<Server>(ex_port);
  RemoteDFS remote (&peer, external_net);
  external_net->establish();

  context.join();
  delete internal_net;
  delete external_net;

  return EXIT_SUCCESS;
}
