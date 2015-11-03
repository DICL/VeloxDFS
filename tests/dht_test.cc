#include "dht/DHTserver.hh"
#include "dht/DHTclient.hh"
#include <thread>
#include <future>
#include <mutex>
#include <algorithm>
#include <vector>

using namespace std;
const int nservers = 1;

int main( ) {
    mutex m1;
    m1.lock();
    auto cache_thread = std::thread ([&] () {
        DHTserver s (nservers);
        s.listen();
        s.accept();

        s.count_query(100); 
        s.count_query(10); 
        s.count_query(1320); 
        s.count_query(12312312); 
        m1.lock();
    });

    string host = "127.0.0.1";
    string key = "hello";

    std::array<DHTclient*, nservers> client_pool {{nullptr}};
    for (auto& c : client_pool) {
      c = new DHTclient(host, nservers);
      c->connect();
      c->detach();
    }

    m1.unlock();
    sleep(2);

    cache_thread.join();
    for (auto& t : client_pool) { delete t; }
  }
//}

//int main () {
//  return UnitTest::RunAllTests();
//}
