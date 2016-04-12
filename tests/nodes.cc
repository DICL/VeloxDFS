#include <nodes/remotedfs.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  std::string input = argv[1];

  RemoteDFS nl;
  nl.establish();

  return context.join();
}
