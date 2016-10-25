#pragma once

#include "message.hh"
#include <boost/asio/streambuf.hpp>
#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace eclipse {
namespace messages {

Message* load_message (boost::asio::streambuf&);
std::string* save_message (Message*);

void send_message(boost::asio::ip::tcp::socket*, eclipse::messages::Message*);
//template <typename T> auto read_reply(boost::asio::ip::tcp::socket*);

template <typename T>
auto read_reply(boost::asio::ip::tcp::socket* socket) {
  using namespace boost::asio;
  char header[17] = {0};
  header[16] = '\0';
  boost::asio::streambuf buf;

  read(*socket, buffer(header, 16));
  size_t size_of_msg = atoi(header);

  read(*socket, buf, transfer_exactly(size_of_msg));

  Message* msg = nullptr;
  msg = load_message(buf);
  T* m = dynamic_cast<T*>(msg);
  return std::unique_ptr<T>(m);
}

} /* messages */ 
}
