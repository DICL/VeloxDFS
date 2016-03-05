#include <nodes/remotedfs.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  string input = argv[1];

  Context context(input);
  context.run();

  RemoteDFS nl (context);
  nl.establish();

  return context.join();
}
