#include "task_manager_router.hh"
#include "../common/context_singleton.hh"
#include "../messages/boost_impl.hh"
#include <functional>
#include <map>
#include <iostream>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
namespace ph = std::placeholders;

// Constructor {{{
TaskManagerRouter::TaskManagerRouter (TaskManager* t_node, Router* router) : RouterDecorator(router) {
	task_manager = t_node;

	using namespace std::placeholders;
	using std::placeholders::_1;
	using std::placeholders::_2;
	auto& rt = routing_table;
	rt.insert({"TaskOperation", bind(&TaskManagerRouter::task_operation, this, _1, _2)});
}

void TaskManagerRouter::task_operation(messages::Message* _m, Channel* tcp_connection){
	auto m = dynamic_cast<messages::TaskOperation*>(_m);
	if(m->operation == messages::TaskOperation::OpType::TASK_INIT){ 
		tasks.insert({m->file, m->lblock_metadata}); // m->file => m->job_id
		task_manager->task_init(m->file, tasks[m->file], m->job_id, m->tmg_id);
	} else if (m->operation == messages::TaskOperation::OpType::TASK_DESTROY){
		tasks.erase(m->file);
	}

}

