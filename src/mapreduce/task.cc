#include "task.hh"
#include <string>

using namespace eclipse;
using namespace std;

int Task::id_counter_ = 0;
Task::Task(TaskType type) {
  id_ = id_counter_++;
  type_ = type;
}
Task::Task(std::string input_path) {
  id_ = id_counter_++;
  input_path_ = input_path;
}
Task::Task(TaskType type, std::string input_path) {
  id_ = id_counter_++;
  type_ = type;
  input_path_ = input_path;
}
Task::Task(int id, TaskType type, std::string input_path) {
  id_ = id;
  type_ = type;
  input_path_ = input_path;
}
Task::~Task() {
}
