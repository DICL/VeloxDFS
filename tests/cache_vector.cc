#include <settings.hh>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const vector<int> input = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

struct Cache_vector_fixture {
  Cache_vector* cache;
  Cache_vector_fixture() {}
};


SUITE(CACHE_VECTOR_TESTS) {
  TEXT_FIXTURE(Cache_vector_fixture, basic) {

    cache = new Cache_vector(5);
      
    for (auto i : input)
      cache->insert (i,i);

    delete cache;
  }
}
