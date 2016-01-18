#include <nodes/peerlocal.hh>
#include <nodes/noderemote.hh>

using namespace eclipse;

SUITE(NODES) {
  TEST(BASIC) {
    //Node n;
    
    Settings setted;
    PeerLocal nl (setted);

    sleep (1);
    //NodeRemote nr (io, "localhost", 10, 0);
  }
}

