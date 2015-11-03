#ifndef __JOB_HH_
#define __JOB_HH_

#include <unordered_multimap>
#include <vector>
#include <string>

using vector;
using string;

enum Map_status {TASK_FINISHED, REQUEST_SENT, RESPOND_RECEIVED};
enum Job_stage {INITIAL_STAGE, MAP_STAGE, REDUCE_STAGE, COMPLETED_STAGE};

class Job {
  public:
    typedef std::unordered_multimap<Task_status, Task>::iterator It;
    typedef std::vector<string> vec_str;

    Job() : id_(-1), num_map_(-1), num_reduce (-1), stage_() {}

    vector<Task> running_tasks () const;
    vector<Task> waiting_tasks () const;
    vector<Task> completed_tasks () const;

    void add_task (Task&);

  public:
    //Setters {{{
    Job& id(int i)             { id_ = i; return *this; }
    Job& num_map(int i)        { num_map_ = i; return *this; }
    Job& num_reduce(int i)     { num_reduce_ = i; return *this; }
    Job& stage(Job_stage i)    { stage_ = i; return *this; }
    Job& status(Map_status i)  { status= i; return *this; }
    Job& inputpaths(vec_str i) { inputpaths_ = i; return *this; }
    Job& peerids(vec_str i)    { peerids_ = i; return *this; }
    Job& numiblocks(vec_str i) { numiblocks_ = i; return *this; }
    Job& argvalues(vec_str i)  { argvalues_ = i; return *this; }

    //Getters
    int id()             const { return id_; }
    int num_map()        const { return num_map_; }
    int num_reduce()     const { return num_reduce_; }
    Job_stage stage()    const { return stage_; }
    Map_status status()  const { return status_; }
    vec_str inputpaths() const { return inputpaths_; }
    vec_str peerids()    const { return peerids_; }
    vec_str numiblocks() const { return numiblocks_; }
    vec_str argvalues()  const { return argvalues_; }
    //}}}

  protected: 
    std::unordered_multimap<Task_status, Task> tasks;
    vec_str inputpaths_, peerids_, numiblocks_, argvalues_;
    int id_, num_map_, num_reduce_;
    Job_stage stage_;
    Map_status status_;
};

vector<Task> Job::running_tasks () const {
  auto interval = tasks.equal_range(Task_status::RUNNING);
  return vector<Task>(interval.first, interval.second);
}

vector<Task> Job::waiting_tasks () const {
  auto interval = tasks.equal_range(Task_status::WAITING);
  return vector<Task>(interval.first, interval.second);
}

vector<Task> Job::completed_tasks () const {
  auto interval = tasks.equal_range(Task_status::COMPLETED);
  return vector<Task>(interval.first, interval.second);
}

#endif
