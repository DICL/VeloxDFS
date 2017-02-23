#include "simple_router.hh"
#include "../common/context_singleton.hh"

using namespace eclipse;
using namespace eclipse::messages;
using namespace std;

// on_read {{{
void SimpleRouter::on_read (Message* m, Channel* s) {
  string type = m->get_type();
  ERROR("I could not find a handler for the message type: %s", type.c_str());
}
// }}}
