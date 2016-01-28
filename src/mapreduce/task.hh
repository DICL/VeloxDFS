#pragma once
#include <string>

namespace eclipse {

enum TaskType {
  kMap,
  kReduce,
};

class Task {
  public:
    Task();
    Task(int id);
    Task(TaskType type);
    Task(std::string input_path);
    Task(TaskType type, std::string input_path);
    Task(int id, TaskType type, std::string input_path);
    ~Task();
    void SetId(int id);
    int GetId();
    void SetType(TaskType type);
    TaskType GetType();
    void SetInputPath(std::string input_path);
    std::string GetInputPath();

  private:
    int id_;
    TaskType type_;
    std::string input_path_;
    static int id_counter_;
};

}
