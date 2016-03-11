#include <iostream>
#include <fstream>
#include <string>
#include "common/context.hh"
#include "directory.hh"

using namespace std;
using namespace eclipse;

int main(int argc, char* argv[])
{
  Context con;
  Directory dir;
  string path = con.settings.get<string>("path.scratch");
  //const int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  //for test
  const int NUM_SERVERS = 1;

  for(int net_id=0; net_id<NUM_SERVERS; net_id++)
  {
    string rmblock = path + "/*";
    //TODO: remote_server[net_id].remove(rmblock.c_str());
    //TODO: remote_server[net_id].dir.init_db();

    //remote node side
    remove(rmblock.c_str());
    dir.init_db();
  }
  return 0;
}
