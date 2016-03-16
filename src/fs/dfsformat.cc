#include <iostream>
#include <dirent.h>
#include <cstdio>
#include "common/context.hh"
#include "directory.hh"

using namespace std;
using namespace eclipse;

int main(int argc, char* argv[])
{
  Context con;
  Directory dir;
  string path = con.settings.get<string>("path.scratch");
  string mpath = con.settings.get<string>("path.metadata") + "/metadata.db";
  
  //const int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  //for test
  const int NUM_SERVERS = 1;

  for(int net_id=0; net_id<NUM_SERVERS; net_id++)
  {
    //TODO: remote_server[net_id].remove(rmblock.c_str());
    //TODO: remote_server[net_id].dir.init_db();

    //remote node side
    DIR *theFolder = opendir(path.c_str());
    struct dirent *next_file;
    char filepath[256];

    while ( (next_file = readdir(theFolder)) != NULL )
    {
      sprintf(filepath, "%s/%s", path.c_str(), next_file->d_name);
      remove(filepath);
    }
    closedir(theFolder);

    remove(mpath.c_str());
    dir.init_db();
  }
  return 0;
}
