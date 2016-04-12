#include <iostream>
#include "dfs.hh"

using namespace std;
using namespace eclipse;

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    cout << "[INFO] dfs put|get|ls|rm|format" << endl;
    return -1;
  }
  else
  {
    DFS dfs;
    dfs.load_settings();
    string op = argv[1];
    if(op.compare("put") == 0)
    {
      dfs.put(argc, argv);
    }
    else if(op.compare("get") == 0)
    {
      dfs.get(argc, argv);
    }
    else if(op.compare("ls") == 0)
    {
      dfs.ls(argc, argv);
    }
    else if(op.compare("rm") == 0)
    {
      dfs.rm(argc, argv);
    }
    else if(op.compare("format") == 0)
    {
      dfs.format(argc, argv);
    }
    else if(op.compare("show") == 0)
    {
      dfs.show(argc, argv);
    }
    else
    {
      cerr << "[ERR] Unknown operation" << endl;
      cout << "[INFO] dfs put|get|ls|rm|format" << endl;
      return -1;
    }
  }
  return 0;
}
