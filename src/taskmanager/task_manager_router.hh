#pragma once
#include "../network/router_decorator.hh"
#include "task_manager.hh"

namespace eclipse {

	
class TaskManagerRouter: public RouterDecorator {
  public:
    TaskManagerRouter(TaskManager*, Router*);
    ~TaskManagerRouter() = default;

    void task_operation(messages::Message*, Channel*);

  private:
    TaskManager* task_manager = nullptr;
	std::map<std::string, struct logical_block_metadata> tasks;
	//int* task_queue_depth;
};

} /* eclipse  */
