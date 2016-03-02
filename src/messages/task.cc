#include  "task.hh"

namespace eclipse {
namespace messages {

std::string Task::get_type() const { return "Task"; }

Task& Task::set_id(int i)   { id = i; return *this; }
Task& Task::set_type(int i) { type = i; return *this; }
Task& Task::set_input_path(std::string i) { 
  input_path = i; 
  return *this;
}

int Task::get_id()                 { return id; }
int Task::get_type()               { return type;}
std::string Task::get_input_path() { return input_path; }

//// serialize {{{
//eclipse::messages::Task* serialize (eclipse::Task* in) {
//  Task* t = new Task();
//  t->id = in->GetId();
//  t->type = in->GetType();
//  t->input_path = in->GetInputPath();
//  return t;
//}
//// }}}
//// deserialize {{{
//eclipse::Task* deserialize (Task* in) {
//  eclipse::Task* t = new eclipse::Task();
//  t->SetId(in->id);
//  t->SetType(static_cast<eclipse::TaskType>(in->type));
//  t->SetInputPath(in->input_path);
//  return t;
//}
//// }}}
}
}
