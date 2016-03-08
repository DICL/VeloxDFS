#include <nodes/remotedfs.hh>
#include <common/context.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {
  Context context;
  context.run();

  RemoteDFS nl (context);
  nl.establish();

  return context.join();
}
