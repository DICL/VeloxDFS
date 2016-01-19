#include "network/topology.hh"
#include "network/ringtopology.hh"
#include "network/meshtopology.hh"
#include "common/logger.hh"
#include <boost/asio.hpp>
#include <stdlib.h>
#include <thread>

using namespace eclipse::network;

const std::vector<std::string> net_ {
  "ferrari",
  "ferrari01",
  "ferrari02"
};

int main (int argc, char **argv) {
    boost::asio::io_service io;
    Logger* logger = Logger::connect ("TEST","LOG_LOCAL6");
    MeshTopology rt1 (io, logger, net_, 9000, atoi(argv[1]));
    rt1.establish();

    io.run();

    while (true) sleep(1);
  
    return 0;
}
