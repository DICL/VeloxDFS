#pragma once

#include "message.hh"
#include <boost/asio/streambuf.hpp>
#include <boost/asio.hpp>
#include <string>
#include <memory>

#include <iostream>
using namespace std;


namespace eclipse {
namespace messages {

Message* load_message (boost::asio::streambuf&);
Message* load_message (std::string&);
std::string* save_message (Message*);

void send_message(boost::asio::ip::tcp::socket*, eclipse::messages::Message*);

template <typename T>
auto read_reply(boost::asio::ip::tcp::socket* socket) {
  using namespace boost::asio;
  char header[17] = {0};
  header[16] = '\0';
  boost::asio::streambuf buf;

  read(*socket, buffer(header, 16));
  size_t size_of_msg = atoi(header);

  size_t l = read(*socket, buf.prepare(size_of_msg));

  Message* msg = nullptr;
  buf.commit(l);
  msg = load_message(buf);
  buf.consume(l);
  T* m = dynamic_cast<T*>(msg);
  return std::unique_ptr<T>(m);
}

} /* messages */ 
}
