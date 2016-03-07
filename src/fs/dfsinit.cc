#include "directory.hh"

using namespace eclipse;

int main(int argc, char* argv[])
{
  Directory dir;
  int NUM_NODE = 1;
  
  for(int net_id=0; net_id<NUM_NODE; net_id++)
  {
    //TODO: Node remote_node = NodeList.lookup(net_id);
    //TODO: remote_node.init_db();
    dir.init_db();
  }
  return 0;
}
