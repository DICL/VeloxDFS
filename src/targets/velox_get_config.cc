#include "../common/context_singleton.hh"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  string query = argv[1]; // Get query

  // Get option
  string opt; 
  if (argc > 2) {
   opt = argv[2];
  }

  if (query == "id") {
    cout << context.id << endl;

  } else if (opt == "-v" ) {
    for (auto elem : GET_VEC_STR(query)) {
      cout << elem << endl;
    }
  } else {
    cout << GET_STR(argv[1]) << endl;
  }

  return EXIT_SUCCESS;
}
