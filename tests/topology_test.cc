#include "network/topology.hh"
#include "network/ringtopology.hh"
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
    RingTopology rt1 (io, net_, 9000, 0);
    RingTopology rt2 (io, net_, 9000, 1);
    RingTopology rt3 (io, net_, 9000, 2);
    rt1.establish();
    rt2.establish();
    rt3.establish();

    auto t1 = std::thread([&]() {
      io.run();
    });
  
    t1.join();
  }

}
