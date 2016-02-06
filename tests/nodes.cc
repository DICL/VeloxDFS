#include <nodes/peerlocal.hh>
#include <nodes/noderemote.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  string input = argv[1];

  Context context(input);
  PeerLocal nl (context);

  nl.establish();
  nl.run();

  sleep(2);

  nl.insert ("KEY", "VALUE");
  nl.request ("KEY", [&] (std::string in) -> void {
      std::cout << "DONE: " << in << std::endl;
      });

  nl.join();

  return 0;
}
