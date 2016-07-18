#include "asyncchannel.hh"
#include "../messages/factory.hh"
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
AsyncChannel::AsyncChannel(tcp::socket* s, tcp::socket* r, NetObserver* node_, int i) : 
  node (node_),
  sender(s),
  receiver(r),
  id(i)
{ 
  if (s == nullptr or r == nullptr) {
    ERROR("NULL pointer passed to asyncchannel %p %p", s, r);
    exit(EXIT_FAILURE);
  }
  is_writing.store(false);
}
AsyncChannel::~AsyncChannel() {
  if (receiver!= nullptr) {
    receiver->close();
    delete receiver;
  }
}
// }}}
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
// do_write_impl {{{
void AsyncChannel::do_write_impl() {
  auto to_write =  messages_queue.front();
  async_write (*sender, buffer(*to_write), transfer_exactly(to_write->size()),
      boost::bind (&AsyncChannel::on_write, this, ph::error, ph::bytes_transferred));
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
  spawn(iosvc, bind(&AsyncChannel::read_coroutine, this, _1));
}
// }}}
// read_coroutine {{{
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf buf;
  boost::system::error_code ec;
  char header [header_size + 1] = {'\0'}; 
  header[16] = '\0';

  try {
    while (true) {
      size_t l = async_read(*receiver, buffer(header, header_size), yield[ec]);
      if (l != (size_t)header_size or ec)  {
        throw std::runtime_error("header size");
      }

      DEBUG("Package have arrived");
      size_t size = atoi(header);
      l = read (*receiver, buf, transfer_exactly(size));
      if (l != size)  {
        throw std::runtime_error("body size");
      }

      Message* msg = nullptr;
      msg = load_message(buf);

      DEBUG("Package has been deserialized");
      node->on_read(msg, id);
      delete msg;
      msg=nullptr;
    }
  } catch (std::exception& e) {
    if (ec == boost::asio::error::eof)
      INFO("AsyncChannel: Closing server socket to client");

    else
      INFO("AsyncChannel: unformed header arrived from host %s, ex: %s", 
          receiver->remote_endpoint().address().to_string().c_str(), e.what());

    node->on_disconnect(nullptr, id);
  }

}
// }}}
