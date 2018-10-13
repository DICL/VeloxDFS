#pragma once
#include "stats_listener.hh"
#include <string>
#include <utility>

namespace eclipse {

class zk_listener: public stats_listener {
  public:
    zk_listener ();
    virtual ~zk_listener () override = default;

    virtual std::vector<std::pair<double, int>> get_io_stats() override;

  private:
    std::string zk_server_addr;
    int zk_server_port;
};

}
