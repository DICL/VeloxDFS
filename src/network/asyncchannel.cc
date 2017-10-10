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
AsyncChannel::~AsyncChannel() {
  socket.close();
}
// }}}
// get_socket {{{
tcp::socket& AsyncChannel::get_socket() {
  return socket;
}
//}}}
// do_write str {{{
void AsyncChannel::do_write(std::shared_ptr<std::string>& str_p) {
  queue_mutex.lock();
  messages_queue.push(str_p);
  queue_mutex.unlock();
  if (!is_writing.exchange(true)) {
    do_write_impl ();
  }
}
//}}}
// do_write {{{
void AsyncChannel::do_write(Message* m) {
  string* str = save_message(m);

  queue_mutex.lock();
  messages_queue.push(shared_ptr<string>(str));
  queue_mutex.unlock();

  if (!is_writing.exchange(true)) {
    do_write_impl ();
  }
}
// }}}
// do_write_buffer {{{
void AsyncChannel::do_write_buffer() {
  if (!is_writing.exchange(true)) {
    do_write_impl();
  }
}
// }}}
// commit{{{
void AsyncChannel::commit(std::shared_ptr<std::string>& str_p) {
  queue_mutex.lock();
  messages_queue.push(str_p);
  queue_mutex.unlock();
}
//}}}
// do_write_impl {{{
void AsyncChannel::do_write_impl() {
  shared_ptr<std::string> to_write;
  queue_mutex.lock();
  to_write = messages_queue.front();
  queue_mutex.unlock();

  host = socket.remote_endpoint().address().to_string().c_str();
  port = socket.remote_endpoint().port();
  async_write (socket, buffer(*to_write), transfer_exactly(to_write->size()),
      boost::bind (&AsyncChannel::on_write, shared_from_this(), ph::error, ph::bytes_transferred));
}
// }}}
// on_write {{{
void AsyncChannel::on_write(const boost::system::error_code& ec, 
    size_t s) {
  if (ec) {
    WARN("Message could not reach err=%s (transferred %lu B) host %s", 
        ec.message().c_str(), s, host.c_str());

    if (ec == boost::asio::error::connection_reset) {
      WARN("Reconnecting to %s %u", host.c_str(), port);

      try {
        socket.close();
        tcp::resolver resolver (context.io);
        tcp::resolver::query query (host, to_string(port));
        tcp::resolver::iterator it(resolver.resolve(query));
        auto ep = make_unique<tcp::endpoint>(*it);
        boost::system::error_code ec;
        auto self(shared_from_this());
        std::function<void(const boost::system::error_code&)> connect_callback = 
          [&, self] (const boost::system::error_code& ec) {

            if (ec) {
              if(ec == boost::asio::error::timed_out) {
                self->get_socket().close();
                socket.async_connect(*ep, connect_callback);
              }

              ERROR("Failed to reconnect err=%s host %s", 
                  ec.message().c_str(), host.c_str());
              exit(EXIT_FAILURE);

            } else {
              do_write_impl();
            }
            };

        socket.async_connect(*ep, connect_callback);
           
      } catch (exception& e) {
        ERROR("Mapper exception %s", e.what());
      } catch (boost::exception& e) {
        ERROR("Mapper exception %s", diagnostic_information(e).c_str());
      }
    }

  } else {

    queue_mutex.lock();
    messages_queue.pop();
    queue_mutex.unlock();

    if (!messages_queue.empty()) {
      do_write_impl ();
    } else {
      // Keep it alive for 10s
      auto t = std::make_shared<boost::asio::deadline_timer>(context.io, boost::posix_time::seconds(10));
      auto self(shared_from_this());
      t->async_wait([t, self] (const boost::system::error_code& ec) { });

      is_writing.exchange(false);
    }
  }
}
// }}}
// do_read {{{
void AsyncChannel::do_read () {
  DEBUG("Connection established, starting to read");
  spawn(context.io, boost::bind(&AsyncChannel::read_coroutine, shared_from_this(), _1));
}
// }}}
// read_coroutine {{{
//! @note This is a coroutine, if you don't know what is it look it up before going crazy. 
//! @todo Fix this exception hell.
//! @date February 9th, 2017
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf buf;
  boost::system::error_code ec;
  char* header = new char[header_size + 1];  // Extra element for strtoul
  bzero(header, header_size + 1); 

  try {
    while (true) {
      auto self(shared_from_this());

      //! Read header of incoming message, we know its size.
      size_t recv = async_read(socket, buffer(header, header_size), yield[ec]);
      if (ec == boost::asio::error::eof)
        break;

      if (recv != (size_t)header_size or ec)
        throw std::runtime_error("header error");

      //! The header gives us the length of the incoming message.
      //! Note, that buf.prepare is the fastest way to read. 
      size_t size = strtoul(header, NULL, 10);
      const auto& read_buffer = buf.prepare(size);

      recv = async_read(socket, read_buffer, yield[ec]);
      if (ec == boost::asio::error::eof)
        break;

      if (recv != size or ec)
        throw std::runtime_error("body error");

      buf.commit(recv);
      unique_ptr<Message> msg {load_message(buf)};
      buf.consume(recv);

      node->on_read(msg.get(), self.get());
    }

  } catch (std::bad_alloc& e) {
    ERROR("Running out of memory");

  } catch (std::exception& e) {
    ERROR("AsyncChannel: unformed message arrived from host %s, ex: %s", 
          socket.remote_endpoint().address().to_string().c_str(), e.what());

  } catch (boost::exception& e) {
    ERROR("read exception %s", diagnostic_information(e).c_str());
  }
  delete[] header;
}
// }}}
