#include "job.hh"
#include "task.hh"
#include <string>
#include <iostream>
#include <cstdlib>

using namespace eclipse;

int Job::id_counter_ = 0;

Job::Job() {
  id_ = id_counter_++;
  tot_num_map_slot_ = DEFAULT_TNMS;
  tot_num_red_slot_ = DEFAULT_TNRS;
  num_map_slot_ = DEFAULT_NMS;
  num_red_slot_= DEFAULT_NRS;
  num_map_task_ = 0;
  num_red_task_ = 0;
}
Job::Job(std::string app_path) {
  Job();
  SetApplicationPath(app_path);
}
Job::~Job() {
}
int Job::GetId() {
  return id_;
}
void Job::SetApplicationPath(std::string app_path) {
  app_path_ = app_path;
}
std::string Job::GetApplicationPath() {
  return app_path_;
}
void Job::SetTotalNumMapSlot(int tot_num_map_slot) {
  tot_num_map_slot_ = tot_num_map_slot;
}
int Job::GetTotalNumMapSlot() {
  return tot_num_map_slot_;
}
void Job::SetTotalNumReduceSlot(int tot_num_red_slot) {
  tot_num_red_slot_ = tot_num_red_slot;
}
int Job::GetTotalNumReduceSlot() {
  return tot_num_red_slot_;
}
void Job::SetNumMapSlot(int num_map_slot) {
  num_map_slot_ = num_map_slot;
}
int Job::GetNumMapSlot() {
  return num_map_slot_;
}
void Job::SetNumReduceSlot(int num_red_slot) {
  num_red_slot_ = num_red_slot;
}
int Job::GetNumReduceSlot() {
  return num_red_slot_;
}
void Job::AddTask(Task* task) {
  if (task->GetType() == kMap) {
    map_tasks_[num_map_task_++] = task;
  }
  else if (task->GetType() == kReduce) {
    red_tasks_[num_red_task_++] = task;
  }
  else {
    std::cout << "[ERROR] type of task has not been setted." << std::endl;
    exit(1);
  }
}
void Job::AddTask(TaskType type, std::string input_path) {
  Task* task = new Task(type, input_path);
  AddTask(task);
}
Task* Job::GetTask(TaskType type, int index) {
  if (type == kMap) {
    if (index >= num_map_task_ || index < 0) {
      std::cout << "[ERROR] index is invalid for GetTask()" << std::endl;
      exit(1);
    }
    return map_tasks_[index];
  }
  else if (type == kReduce) {
    if (index >= num_red_task_ || index < 0) {
      std::cout << "[ERROR] index is invalid for GetTask()" << std::endl;
      exit(1);
    }
    return red_tasks_[index];
  }
  else {
    std::cout << "[ERROR] type is invalid for GetTask()" << std::endl;
    exit(1);
  }
}
