#include <iostream>
#include "dfs.hh"

using namespace std;
using namespace eclipse;

int main(int argc, char* argv[]) {
  if (argc >= 2) {
    DFS dfs;
    dfs.load_settings();
    string op = argv[1];
    if (op.compare("put") == 0) {
      dfs.put(argc, argv);
      return 0;
    }
    else if (op.compare("get") == 0) {
      dfs.get(argc, argv);
      return 0;
    }
    else if (op.compare("cat") == 0) {
      dfs.cat(argc, argv);
      return 0;
    }
    else if (op.compare("ls") == 0) {
      dfs.ls(argc, argv);
      return 0;
    }
    else if (op.compare("rm") == 0) {
      dfs.rm(argc, argv);
      return 0;
    }
    else if (op.compare("format") == 0) {
      dfs.format(argc, argv);
      return 0;
    }
    else if (op.compare("show") == 0) {
      dfs.show(argc, argv);
      return 0;
    }
    else if (op.compare("partial_get") == 0) {
      dfs.DFS::partial_get(argc, argv);
      return 0;
    }
  }
  cerr << "[ERR] Unknown operation" << endl;
  cout << "[INFO] dfs put|get|cat|ls|rm|format|partial_get" << endl;
  return -1;
}
