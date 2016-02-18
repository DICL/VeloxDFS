#include "machine.hh"

namespace eclipse {

Machine::Machine(int id_) : id(id_) { }
int Machine::get_id() const { return id; }

}
