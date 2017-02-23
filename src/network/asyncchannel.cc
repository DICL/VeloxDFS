#include "asyncchannel.hh"
#include "../messages/factory.hh"
#include "../common/context_singleton.hh"
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <boost/bind.hpp>
#include <unistd.h>
#include <stdexcept>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace ph = boost::asio::placeholders;
using namespace std;
using namespace boost::asio;
using namespace eclipse::network;
using namespace eclipse::messages;
using namespace boost::system;
using namespace boost::archive;

// constructor {{{
AsyncChannel::AsyncChannel(NetObserver* node_) : 
  node (node_),
  socket(context.io),
  iosvc(context.io)
{ 
  is_writing.store(false);
}
// }}}
// get_socket {{{
tcp::socket& AsyncChannel::get_socket() {
  return socket;
}
//}}}
// do_write str {{{
void AsyncChannel::do_write(std::shared_ptr<std::string>& str_p) {
  messages_queue.push(str_p);
  if (!is_writing.exchange(true)) {
    do_write_impl ();
  }
}
//}}}
// do_write {{{
void AsyncChannel::do_write(Message* m) {
  string* str = save_message(m);

  messages_queue.push(shared_ptr<string>(str));
  if (!is_writing.exchange(true)) {
    do_write_impl ();
  }
}
// }}}
// do_write_buffer {{{
void AsyncChannel::do_write_buffer() {
  do_write_impl();
}
// }}}
// commit{{{
void AsyncChannel::commit(std::shared_ptr<std::string>& str_p) {
  messages_queue.push(str_p);
}
//}}}
// do_write_impl {{{
void AsyncChannel::do_write_impl() {
  auto to_write =  messages_queue.front();
  async_write (socket, buffer(*to_write), transfer_exactly(to_write->size()),
      boost::bind (&AsyncChannel::on_write, shared_from_this(), ph::error, ph::bytes_transferred));
}
// }}}
// on_write {{{
void AsyncChannel::on_write(const boost::system::error_code& ec, 
    size_t s) {
  if (ec) {
    INFO("Message could not reach err=%s", 
        ec.message().c_str());

    do_write_impl();
  } else {
    messages_queue.pop();

    if (!messages_queue.empty()) {
      do_write_impl ();
    } else {
      is_writing.exchange(false);
    }
  }
}
// }}}
// do_read {{{
void AsyncChannel::do_read () {
  DEBUG("Connection established, starting to read");
  spawn(iosvc, boost::bind(&AsyncChannel::read_coroutine, shared_from_this(), _1));
}
// }}}
// read_coroutine {{{
//! @note This is a coroutine, if you don't know what is it look it up before going crazy. 
//! @todo Fix this exception hell.
//! @date February 9th, 2017
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf buf;
  boost::system::error_code ec;
  char header [header_size + 1] = {'\0'}; 

  try {
    while (true) {
      //! Read header of incoming message, we know its size.
      size_t recv = async_read(socket, buffer(header, header_size), yield[ec]);

      if (recv != (size_t)header_size or ec)
        throw std::runtime_error("header error");

      DEBUG("Header has arrived");

      //! The header gives us the length of the incoming message.
      //! Note, that buf.prepare is the fastest way to read. 
      size_t size = strtoul(header, NULL, 10);
      recv = async_read(socket, buf.prepare(size), yield[ec]); // :TODO: watchout async

      if (recv != size or ec)
        throw std::runtime_error("body error");

      buf.commit(recv);
      unique_ptr<Message> msg {load_message(buf)};
      buf.consume(recv);

      DEBUG("Package has been deserialized");
      node->on_read(msg.get(), this);
    }
  } catch (std::exception& e) {
    if (ec == boost::asio::error::eof)
      INFO("AsyncChannel: Closing server socket to client");

    else
      ERROR("AsyncChannel: unformed message arrived from host %s, ex: %s", 
          socket.remote_endpoint().address().to_string().c_str(), e.what());
  }

}
// }}}
