#include "executor.hh"
#include "../messages/factory.hh"
#include "../messages/boost_impl.hh"
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <boost/bind.hpp>

namespace ph = boost::asio::placeholders;
using namespace eclipse;
using namespace eclipse::messages;
using namespace boost::asio;
using namespace std;

namespace eclipse {

// Constructor {{{
Executor::Executor(Context& context) : MR_traits (context) {

}

Executor::~Executor() {

}
// }}}
// process_message (Task* m) {{{
template<> void Executor::process_message (messages::Task* m) {
  if (m->type == 0) {
    DL_loader loader (m->library);
    auto _map_ = loader.load_function(m->func_name);

    auto file = peer_cache.lookup(m->input_path);
    while (file.eof()) {
      auto& key_value = _map_ (file.read_line());
      peer_cache.insert_intermediate(key_value, m->next_id);
    }
  }
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

  if (type == "Task") {
    auto m_ = dynamic_cast<messages::Task*>(m);
    process_message(m_);

  } else if (type == "Control") {
    auto m_ = dynamic_cast<Control*>(m);
    process_message(m_);
  }
}
// }}}
// action {{{
void Executor::action (tcp::socket* sock) {
  do_read(sock);
}
// }}}
// do_read {{{
void Executor::do_read (tcp::socket* sock) {
  async_read (*sock, buffer(inbound_header), 
      transfer_exactly(header_size),
      boost::bind(&Executor::on_read_header, this, 
        ph::error, ph::bytes_transferred, sock));
}
// }}}
// on_read_header {{{
void Executor::on_read_header (const error_code& ec, 
    size_t s, tcp::socket* sock) {

    size_t size = atoi(inbound_header);
    async_read (*sock, inbound_data, 
      transfer_exactly (size), boost::bind(
        &Executor::on_read_body, this, 
        ph::error, ph::bytes_transferred, sock));

}
// }}}
// on_read_body {{{
void Executor::on_read_body (const error_code& ec,
    size_t s, tcp::socket* sock) {
  if (!ec)  {
    logger->info ("Message arrived size=%d");

    std::string str((istreambuf_iterator<char>(&inbound_data)), 
                     istreambuf_iterator<char>());

    Message* msg = nullptr;
    msg = load_message(str);
    process_message(msg);
    delete msg;
    do_read(sock);
  }

  size_t size = atoi(inbound_header);
  async_read (*sock, inbound_data, 
    transfer_exactly (size), boost::bind(
      &Executor::on_read_body, this, 
      ph::error, ph::bytes_transferred, sock));
}
//}}}}

} /* eclipse  */
