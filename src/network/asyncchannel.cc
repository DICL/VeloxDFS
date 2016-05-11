#include "asyncchannel.hh"
#include "../messages/factory.hh"
#include <string>
#include <sstream>
#include <iomanip>
#include <istream>
#include <boost/bind.hpp>
#include <unistd.h>
#include <stdexcept>

namespace ph = boost::asio::placeholders;
using namespace std;
using namespace boost::asio;
using namespace eclipse::network;
using namespace eclipse::messages;
using namespace boost::system;

// constructor {{{
AsyncChannel::AsyncChannel(tcp::socket* s, tcp::socket* r, NetObserver* node_, int i) : 
  node (node_),
  sender(s),
  receiver(r),
  id(i)
{ 
  is_writing.store(false);
}
AsyncChannel::~AsyncChannel() {
  if (receiver!= nullptr) {
    receiver->close();
    delete receiver;
  }
}
// }}}
// do_write {{{
void AsyncChannel::do_write (Message* m) {
  string str = save_message (m);
  if (m == nullptr)
    logger->error ("Null pointer to be sent");

  stringstream ss; 
  ss << setfill('0') << setw(header_size) << str.length() << str;
  //string* to_write = new string(ss.str());
  // TODO messages_queue may not be thread-safe.
  messages_queue.emplace (new string(ss.str()));
  if (!is_writing.exchange(true)) {
    do_write_impl ();
  }
}
// }}}
// do_write_impl {{{
void AsyncChannel::do_write_impl () {
  auto to_write =  messages_queue.front();
  async_write (*sender, buffer(*to_write), boost::asio::transfer_exactly(to_write->length()) ,boost::bind (&AsyncChannel::on_write, 
        this, ph::error, ph::bytes_transferred));
}
// }}}
// on_write {{{
void AsyncChannel::on_write (const boost::system::error_code& ec, 
    size_t s) {
  if (ec) {
    logger->info ("Message could not reach err=%s", 
        ec.message().c_str());

    //sleep(10);
    do_write_impl();
  } else {
    delete messages_queue.front();
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
  logger->info("Connection established, starting to read");
  spawn(iosvc, bind(&AsyncChannel::read_coroutine, this, _1));
}
// }}}
// read_coroutine {{{
void AsyncChannel::read_coroutine (yield_context yield) {
  boost::asio::streambuf body; 
  boost::system::error_code ec;
  char header [header_size + 1]; 
  header[16] = '\0';

  try {
    while (true) {
      size_t l = async_read (*receiver, buffer(header, header_size), yield[ec]);
      if (l != (size_t)header_size or ec)  {
        logger->info ("HEADER size %d", l);
        throw std::runtime_error("header size");
      }

      size_t size = atoi(header);
      l = read (*receiver, body.prepare(size));
      if (l != size)  {
        logger->info ("Body size %d != %d", l, size);
        throw std::runtime_error("body size");
      }
      //if (ec) throw 1;

      body.commit (l);
      string str ((istreambuf_iterator<char>(&body)), 
          istreambuf_iterator<char>());
      body.consume (l);

      Message* msg = nullptr;
      msg = load_message(str);
      node->on_read(msg, id);
      delete msg;
      msg=nullptr;
    }
  } catch (std::exception& e) {
    if (ec == boost::asio::error::eof)
      logger->info ("AsyncChannel: Closing server socket to client");
    else
      logger->info ("AsyncChannel: unformed header arrived from host %s, ex: %s", 
          receiver->remote_endpoint().address().to_string().c_str(), e.what());


      node->on_disconnect(nullptr, id);
  }
}
// }}}
