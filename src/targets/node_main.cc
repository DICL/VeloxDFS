#include <nodes/remotedfs.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  RemoteDFS nl;
  nl.establish();

  return context.join();
}
