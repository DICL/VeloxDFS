#include <nodes/peerlocal.hh>
#include <nodes/noderemote.hh>
#include <common/settings.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  string input = argv[1];

  Settings setted (input);
  PeerLocal nl (setted);

  nl.establish();

  nl.run();


  while (true) 
    sleep (1); 

  return 0;
}
