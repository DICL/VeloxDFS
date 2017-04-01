#include "../client/cli_driver.hh"
#include <iostream>

using namespace velox;
using namespace std;

int main(int argc, char** argv) {
  cli_driver cli;
  cli.parse_args(argc, argv);
  return EXIT_SUCCESS;
}
