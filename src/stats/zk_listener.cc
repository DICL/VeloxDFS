#include "zk_listener.hh"
#include <zookeeper/zookeeper.h>
#include "../common/context_singleton.hh"
#include <string>

#define ZK_BUFFER_LEN 512

using namespace eclipse;
using namespace std;

// Convenience free functions {{{
namespace {

zhandle_t* connect_to_zk (string host, int port) {
  zhandle_t* zh = nullptr;
  string addr_port = host + ":" + to_string(port);

  zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
  zh = zookeeper_init(addr_port.c_str(), NULL, 10000, 0, 0, 0);

  return zh;
}

void report_error(string path, int rc) {
  ERROR("ZK_LISTENER cannot open path %s with error code %i", path.c_str(), zerror(rc));
}

}
// }}}
// ctor {{{
zk_listener::zk_listener() {
  zk_server_addr = GET_STR("addons.zk.addr");
  zk_server_port = GET_INT("addons.zk.port");
}

// }}}
// get_io_stats {{{
std::vector<double> zk_listener::get_io_stats() {
  char buffer[ZK_BUFFER_LEN] {0};
  int buflen = ZK_BUFFER_LEN;
  int rc = 0;
  struct Stat stat;
  vector<double> ret;

  auto zh = connect_to_zk(zk_server_addr, zk_server_port);

  if (zh) {
    rc = zoo_get(zh, "/stats/io", 0, buffer, &buflen, &stat);

    if (!rc) {
      int n_childrens = stat.numChildren;
      ret.reserve(n_childrens);

      for (int i = 0; i < n_childrens; i++) {
        bzero(buffer, ZK_BUFFER_LEN);
        bzero(&stat, sizeof(struct Stat));
        string children_path = "/stats/io/" + to_string(i);

        rc = zoo_get(zh, children_path.c_str(), 0, buffer, &buflen, &stat);

        if (!rc) {
          ret.push_back(atof(buffer) * .01);  // values from 0.00-1.00

        // Error reporting for child ZNODE
        } else {
          report_error(children_path, rc);
        }
      }

    // Error reporting for main ZNODE
    } else {
      report_error("/stats/io", rc);
    }

  // Error reporting for main connection
  } else {
    ERROR("ZK_LISTENER cannot connect to zk server%s:%i", zk_server_addr.c_str(),
        zk_server_port);
  }

  zookeeper_close(zh);

  return move(ret);
}
// }}}
