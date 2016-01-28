#include "task.hh"
#include <string>

using namespace eclipse;
using namespace std;

int Task::id_counter_ = 0;
Task::Task() {
  id_ = id_counter_++;
}
Task::Task(int id) {
  SetId(id);
}
Task::Task(TaskType type) {
  Task();
  SetType(type);
}
Task::Task(std::string input_path) {
  Task();
  SetInputPath(input_path);
}
Task::Task(TaskType type, std::string input_path) {
  Task();
  SetType(type);
  SetInputPath(input_path);
}
Task::Task(int id, TaskType type, std::string input_path) {
  SetId(id);
  SetType(type);
  SetInputPath(input_path);
}
Task::~Task() {
}
void Task::SetId(int id) {
  id_ = id;
}
int Task::GetId() {
  return id_;
}
void Task::SetType(TaskType type) {
  type_ = type;
}
TaskType Task::GetType() {
  return type_;
}
void Task::SetInputPath(std::string input_path) {
  input_path_ = input_path;
}
std::string Task::GetInputPath() {
  return input_path_;
}
