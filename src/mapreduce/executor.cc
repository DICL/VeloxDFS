#include "nodeexecutor.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"

using namespace eclipse;
using namespace eclipse::messages;
using namespace boost::asio;
using namespace std;

namespace eclipse {

Executor::Executor() {

}

Executor::~Executor() {

}
// process_message (Job* m) {{{
template<> void Executor::process_message (Task* m) {
}
// }}}
// process_message (Control* m) {{{
template<> void Executor::process_message (Control* m) {
  switch (m->type) {
    case messages::SHUTDOWN:
      exit(EXIT_SUCCESS);
      break;

    case messages::RESTART:
      break;

      //    case PING:
      //      process_ping (m);
      //      break;
  }
}
// }}}
// process_message (Message*) {{{
template<> void Executor::process_message (Message* m) {
  string type = m->get_type();

  if (type == "Job") {
    auto m_ = dynamic_cast<Control*>(m);
    process_message(m_);

  } else if (type == "KeyRequest") {
    auto m_ = dynamic_cast<KeyRequest*>(m);
    process_message(m_);
  }
}
// }}}
// load {{{
bool Executor::load (std::string) {
}
// }}}

} /* eclipse  */
