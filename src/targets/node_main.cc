#include <sys/resource.h>
#include <common/context_singleton.hh>
#include <network/server_handler.hh>
#include <network/client_handler.hh>
#include <network/simple_router.hh>
#include <fileleader/file_leader_router.hh>
#include <blocknode/block_node_router.hh>

#ifdef LOGICAL_BLOCKS_FEATURE
#include <src/stats/io_monitor_invoker.hh>
#endif

#include <memory>

using namespace eclipse;
using namespace std;

int main (int argc, char ** argv) {

  struct rlimit core_limits;
  core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
  setrlimit(RLIMIT_CORE, &core_limits);

  uint32_t ex_port = GET_INT("network.ports.client");

  auto internal_net = make_unique<network::ClientHandler> (ex_port);
  auto external_net = make_unique<network::ServerHandler> (ex_port);

  FileLeader file_leader(internal_net.get());
  BlockNode block_node(internal_net.get());

  // Decorator pattern: I want FileLeader and Block node on the same network
  auto router = make_unique<FileLeaderRouter>(&file_leader, new BlockNodeRouter(&block_node, new SimpleRouter()));

  external_net->attach(router.get());

  external_net->establish();

#ifdef LOGICAL_BLOCKS_FEATURE
//  invoke_io_reporter();
#endif

  context.run();
  context.join();

  return EXIT_SUCCESS;
}
