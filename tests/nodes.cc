#include <nodes/peer.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  string input = argv[1];

  Context context(input);
  context.run();

  Peer nl (context);
  nl.establish();

  nl.insert ("KEY", "VALUE");
  nl.request ("KEY", [&] (std::string in) -> void {
      std::cout << "DONE: " << in << std::endl;
      });

  return context.join();
}
