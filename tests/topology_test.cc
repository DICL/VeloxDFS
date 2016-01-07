#include "network/topology.hh"
#include "network/ringtopology.hh"
#include "network/meshtopology.hh"
#include <boost/asio.hpp>
#include <thread>

using namespace eclipse::network;

const std::vector<std::string> net_ {
  "127.0.0.1",
  "127.0.0.1",
  "127.0.0.1"
};

SUITE(TOPOLOGY) {
  TEST(RING) {
    boost::asio::io_service io;
    RingTopology rt1 (io, net_, 9001, 0);
    rt1.establish();

//    rt1.close();

  }
  TEST(MESH) {
    boost::asio::io_service io;
    MeshTopology mt1 (io, net_, 9010, 0);
    mt1.establish();

    mt1.close();
  }
}
