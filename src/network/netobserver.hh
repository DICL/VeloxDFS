#pragma once

#include "../messages/message.hh"
#include <boost/asio.hpp>

namespace eclipse {
namespace network {

using boost::asio::ip::tcp;
struct NetObserver {
  virtual ~NetObserver() { };
  virtual void on_connect(tcp::socket*) = 0;
  virtual void on_accept(tcp::socket*) = 0;
  virtual void on_disconnect(tcp::socket*, int) = 0;
  virtual void on_read (messages::Message*, int) = 0;
};

}
}
