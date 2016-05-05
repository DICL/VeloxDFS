#include <common/context.hh>
#include <network/p2p.hh>
#include <network/server.hh>
#include <network/asyncnetwork.hh>
#include <nodes/router.hh>
#include <nodes/dio.hh>
#include <nodes/dfs.hh>
#include <string>

using namespace eclipse;
using namespace eclipse::network;

int main (int argc, char ** argv) {
  int in_port = context.settings.get<int>("network.ports.internal");
  int ex_port = context.settings.get<int>("network.ports.client");

  Network* internal_net = new AsyncNetwork<P2P>(in_port);
  DIO dio (internal_net);
  DFS dfs (&dio);
  Router router (internal_net, &dfs);
  internal_net->establish();

  Network* external_net = new AsyncNetwork<Server>(ex_port);
  Router router2 (external_net, &dfs);
  external_net->establish();

  context.join();
  delete internal_net;
  delete external_net;

  return EXIT_SUCCESS;
}
