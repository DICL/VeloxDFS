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
      return EXIT_SUCCESS;
    }
    else if (op.compare("get") == 0) {
      dfs.get(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("cat") == 0) {
      dfs.cat(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("ls") == 0) {
      dfs.ls(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("rm") == 0) {
      dfs.rm(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("format") == 0) {
      dfs.format(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("show") == 0) {
      dfs.show(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("pget") == 0) {
      dfs.DFS::pget(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("update") == 0) {
      dfs.DFS::update(argc, argv);
      return EXIT_SUCCESS;
    }
    else if (op.compare("append") == 0) {
      dfs.DFS::append(argc, argv);
      return EXIT_SUCCESS;
    }
  }
  cerr << "[ERR] Unknown operation" << endl;
  cout << "[INFO] dfs put|get|cat|ls|rm|format|pget|update|append" << endl;
  return EXIT_FAILURE;
}
