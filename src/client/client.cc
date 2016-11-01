#include "dfs.hh"
#include <iostream>

using namespace velox;
using namespace std;

int main(int argc, char** argv) {
  std::vector<std::string> arguments(argv, argv + argc);
  DFS dfs;
  dfs.load_settings();
  string op = arguments[1];

  if (op == "put") {
    dfs.put(arguments);

  } else if (op == "get") {
    dfs.get(arguments);

  } else if (op == "cat") {
    dfs.cat(arguments);

  } else if (op == "ls") {
    dfs.ls(arguments);

  } else if (op == "rm") {
    dfs.rm(arguments);

  } else if (op == "format") {
    dfs.format();

  } else if (op == "show") {
    dfs.show(arguments);

  } else if (op == "pget") {
    dfs.pget(arguments);

  } else if (op == "update") {
    dfs.update(arguments);

  } else if (op == "append") {
    dfs.append(arguments);

  } else {
    cerr << "[ERR] Unknown operation" << endl;
    cout << "[INFO] dfs put|get|cat|ls|rm|format|pget|update|append" << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
