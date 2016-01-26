#pragma once
#include "task.hh"
#include <string>
#include <vector>

#define DEFAULT_TNMS 8*16;
#define DEFAULT_TNRS 8*16;
#define DEFAULT_NMS 8*16;
#define DEFAULT_NRS 8*16;

namespace eclipse {

class Job {
  private:
    int id_;
    static int id_counter_;
    std::string app_path_;
    // The total number of worker thread allowed at a time over the whole
    // system.
    // Default is the total number of available worker thread over the whole
    // system.
    int tot_num_map_slot_;
    int tot_num_red_slot_;
    // The number of woker thread allowed at a time within a node.
    // Default is the total number of available worker thread within a node.
    int num_map_slot_;
    int num_red_slot_;
    // The total number of map/reduce tasks.
    int num_map_task_;
    int num_red_task_;
    std::vector<Task*> map_tasks_;
    std::vector<Task*> red_tasks_;

  public:
    Job();
    Job(std::string app_path);
    ~Job();
    int GetId();
    void SetApplicationPath(std::string app_path);
    std::string GetApplicationPath();
    void SetTotalNumMapSlot(int tot_num_map_slot);
    int GetTotalNumMapSlot();
    void SetTotalNumReduceSlot(int tot_num_red_slot);
    int GetTotalNumReduceSlot();
    void SetNumMapSlot(int num_map_slot);
    int GetNumMapSlot();
    void SetNumReduceSlot(int num_red_slot);
    int GetNumReduceSlot();
    void AddTask(Task* task);
    void AddTask(TaskType type, std::string input_path);
    Task* GetTask(TaskType type, int index);
};

}
