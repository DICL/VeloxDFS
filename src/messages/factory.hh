#pragma once

#include "boost_impl.hh"
#include "../common/histogram.hh"

#include <string>
#include <functional>
#include <boost/asio/streambuf.hpp>

namespace network {
  void load_n (boost::asio::streambuf&, std::function<void(Message*)>);

  void operator<< (std::string, Message*);
  void operator<< (Message*, std::string);
  void operator<< (Message*, boost::asio::streambuf&);
  void operator<< (Histogram&, Message&);

  Message* load_message (std::string);
  std::string save_message (Message*);
} /* network */ 
