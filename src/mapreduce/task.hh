#pragma once
#include <string>

namespace eclipse {

enum TaskType {
  kMap,
  kReduce,
};


class Task {
  private:
    int id_;
    TaskType type_;
    std::string input_path_;
    static int id_counter_;

  public:
    Task() { id_ = id_counter_++; }
    Task(int id) { id_ = id; }
    Task(TaskType type);
    Task(std::string input_path);
    Task(TaskType type, std::string input_path);
    Task(int id, TaskType type, std::string input_path);
    ~Task();
    void SetId(int id);
    int GetId();
    void SetType(TaskType type) { type_ = type; }
    TaskType GetType() { return type_; }
    void SetInputPath(std::string input_path) { input_path_ = input_path; }
    std::string GetInputPath() { return input_path_; }
};

}
