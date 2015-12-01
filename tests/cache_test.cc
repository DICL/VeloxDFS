#include "cache/cache.hh"

const int NUM_OF_TEST1_RECORDS = 100;
const int NUM_OF_TEST2_RECORDS = 100;
const int TEST2_CACHE_CAPACITY = 50;

using namespace eclipse;

SUITE(CACHE_TEST) {
  TEST(SimplePut) {
    lru_cache<int, int> cache(1);
    cache.put(7, 777);
    CHECK(cache.exists(7));
    CHECK_EQUAL(777, cache.get(7));
    CHECK_EQUAL(1, cache.size());
  }

  TEST(MissingValue) {
    lru_cache<int, int> cache(1);
    CHECK_THROW(cache.get(7), std::range_error);
  }

  TEST(KeepsAllValuesWithinCapacity) {
    lru_cache<int, int> cache(TEST2_CACHE_CAPACITY);

    for (int i = 0; i < NUM_OF_TEST2_RECORDS; ++i) {
      cache.put(i, i);
    }

    for (int i = 0; i < NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; ++i) {
      CHECK(not cache.exists(i));
    }

    for (int i = NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; i < NUM_OF_TEST2_RECORDS; ++i) {
      CHECK(cache.exists(i));
      CHECK_EQUAL(i, cache.get(i));
    }

    size_t size = cache.size();
    CHECK_EQUAL(TEST2_CACHE_CAPACITY, size);
  }
}

int main () {
  return UnitTest::RunAllTests();
}
