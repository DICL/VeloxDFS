#include <nodes/peerlocal.hh>
#include <nodes/noderemote.hh>

using namespace eclipse;

SUITE(NODES) {
  TEST(BASIC) {
    //Node n;
    
    std::string input (
"{\"name\":\"Raven cluster eclipseMR file\",\"max_job\":1024,\"log\":{\"type\":\"LOG_LOCAL6\",\"name\":\"ECLIPSE\"},\"path\":{\"filesystem\":\"/home/vicente/eclipse_storage/\",\"scratch\":\"/scratch/vicente/storage\",\"ipc\":\"/scratch/vicente/socket\"},\"cache\":{\"numbin\":100,\"size\":200000,\"concurrency\":1},\"network\":{\"port_mapreduce\":8008,\"port_cache\":8009,\"port_dht\":8010,\"topology\":\"mesh\",\"iface\":\"em1\",\"master\":\"10.20.13.123\",\"nodes\":[\"10.20.13.123\",\"10.20.13.124\",\"10.20.13.125\"]}}");
    Settings setted (input);
    PeerLocal nl (setted);

    sleep (1);
    //NodeRemote nr (io, "localhost", 10, 0);
  }
}

