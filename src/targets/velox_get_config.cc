#include "../common/context_singleton.hh"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  string query = argv[1];

  if (query == "id") {
    cout << context.id << endl;

  } else {
    cout << GET_STR(argv[1]) << endl;
  }

  return EXIT_SUCCESS;
}
