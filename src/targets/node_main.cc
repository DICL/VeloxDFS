#include <nodes/remotedfs.hh>
#include <common/context_singleton.hh>
#include <network/p2p.hh>
#include <network/server.hh>
#include <network/asyncnetwork.hh>
#include <memory>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  int in_port = context.settings.get<int>("network.ports.internal");
  int ex_port = context.settings.get<int>("network.ports.client");

  auto internal_net = make_unique<network::AsyncNetwork<P2P>> (in_port);
  PeerDFS peer (internal_net.get());
  internal_net->establish();

  auto external_net = make_unique<network::AsyncNetwork<Server>> (ex_port);
  RemoteDFS remote (&peer, external_net.get());
  external_net->establish();

  context.join();

  return EXIT_SUCCESS;
}
