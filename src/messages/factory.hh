#pragma once

#include "message.hh"
#include <boost/asio/streambuf.hpp>
#include <string>

namespace eclipse {
namespace messages {

Message* load_message (boost::asio::streambuf&);
std::string* save_message (Message*);

} /* messages */ 
}
