#include "task.hh"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>

using namespace std;

typedef pair<Task_status, Task> pair_task ;

struct TaskFix: public Task {
  TaskFix() { }
};

SUITE(TASK_TESTS) {
  TEST_FIXTURE(TaskFix, basic) {
    fd(10).id(2).status(Task_status::RUNNING);
    inputpaths(vector<string> {"hello", "bye"});
    CHECK_EQUAL(fd(), 10);
    CHECK_EQUAL(id(), 2);
    CHECK(status() == Task_status::RUNNING);
    CHECK_EQUAL(inputpaths()[0], "hello");
    CHECK_EQUAL(inputpaths()[1], "bye");
  }
  TEST(multimaptest) {
    unordered_multimap<Task_status, Task> mymap;
    {
      Task& a = Task().fd(10).id(2).status(Task_status::RUNNING);
      Task& b = Task().fd(20).id(3).status(Task_status::RUNNING);

      a.inputpaths(vector<string> {"hello", "bye"});

      mymap.emplace (Task_status::RUNNING, a);
      mymap.emplace (Task_status::RUNNING, b);
    }

    auto p = mymap.equal_range(Task_status::RUNNING);
    vector<pair_task> v (p.first, p.second);
    CHECK_EQUAL(v[0].second.fd(), 20);
    CHECK_EQUAL(v[0].second.id(), 3);
    CHECK_EQUAL(v[1].second.fd(), 10);
    CHECK_EQUAL(v[1].second.id(), 2);
  }
}

int main () {
  return UnitTest::RunAllTests();
}
