#include <settings.hh>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct Settings_fixture: public Settings {
  Settings_fixture () { }
};

SUITE(SETTING_TESTS) {
  TEST_FIXTURE(Settings_fixture, basic) {
    load();

    CHECK_EQUAL (get<int> ("network.ports.client"), 8008);
 //   CHECK_EQUAL (get<string> ("network.master"), "192.168.1.201");
    vector<string> test = get<vector<string> >("network.nodes");

    int i = 1;
    for (auto it = test.begin(); it != test.end(); it++, i++) {
      ostringstream tmp;
      tmp << "192.168.1." << i; 
      string _ip = tmp.str();
//      CHECK_EQUAL (*it, _ip);
    }
  }
}
