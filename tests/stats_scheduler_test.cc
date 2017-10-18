#include <settings.hh>
#include <iostream>
#include <sstream>
#include <vector>
#include <messages/filedescription.hh>
#include <common/histogram.hh>
#include <stats/logical_blocks_scheduler.hh>

using namespace std;
using namespace eclipse::logical_blocks_schedulers;
using namespace eclipse::messages;

namespace {

struct listener_1_busy : public eclipse::stats_listener {
  listener_1_busy() = default;
  ~listener_1_busy() = default;

  std::vector<double> get_io_stats() {
    return {0.01, 0.01, 0.01, 0.99};
  }
};

struct stats_fixture {
  std::vector<std::string> nodes = {"1", "2", "3", "4"};
  map<string, string> opts;
  FileDescription fd;

  stats_fixture () { 
    opts["alpha"] = "0.5";
    opts["beta"]  = "0.5";

    fd.name = "file";
    fd.blocks = {"file_1", "file_2", "file_3", "file_4", "file_5"};
    fd.hash_keys = {123123, 124123, 32323, 4242, 424245};
    fd.block_size = {123123, 124123, 32323, 4242, 424245};
    fd.block_hosts= {"1", "2", "3", "4", "4"};
  }
};

}

SUITE(STATS_TESTS) {

  TEST_FIXTURE(stats_fixture, simple_test) {
    Histogram boundaries (4, 100);
    auto scheduler = scheduler_factory("scheduler_simple", &boundaries, opts);
    scheduler->listener.reset(new listener_1_busy());
    scheduler->generate(fd, nodes);

    CHECK_EQUAL(fd.logical_blocks.size(), 4);
    CHECK_EQUAL(fd.logical_blocks[0].physical_blocks.size(), 1);
    CHECK_EQUAL(fd.logical_blocks[1].physical_blocks.size(), 1);
    CHECK_EQUAL(fd.logical_blocks[2].physical_blocks.size(), 1);
    CHECK_EQUAL(fd.logical_blocks[3].physical_blocks.size(), 2);
  }

  TEST_FIXTURE(stats_fixture, score_based_tests) {
    Histogram boundaries (4, 100);
    auto scheduler = scheduler_factory("scheduler_score_based", &boundaries, opts);
    scheduler->listener.reset(new listener_1_busy());
    scheduler->generate(fd, nodes);

    CHECK_EQUAL(3, fd.logical_blocks.size());
    CHECK_EQUAL(1, fd.logical_blocks[0].physical_blocks.size());
    CHECK_EQUAL(2, fd.logical_blocks[1].physical_blocks.size());
    CHECK_EQUAL(2, fd.logical_blocks[2].physical_blocks.size());
  }
}
