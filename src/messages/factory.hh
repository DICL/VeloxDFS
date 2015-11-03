#pragma once

#include "message.hh"
#include "../common/histogram.hh"

#include <string>
#include <functional>
#include <boost/asio/streambuf.hpp>

namespace network {
  void load_n (boost::asio::streambuf&, std::function<void(Message*)>);

  void operator<< (std::string, Message*);
  void operator<< (Message*, boost::asio::streambuf&);
  void operator<< (Histogram&, Message&);
} /* network */ 
