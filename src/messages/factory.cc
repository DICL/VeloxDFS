#include "factory.hh"
#include "../common/context_singleton.hh"

#include <sstream>
#include <iostream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace boost::asio;
using namespace boost::archive;
using namespace std;

namespace eclipse {
namespace messages {

Message* load_message (std::string& str) {
  Message* m; 
  if (GET_STR("network.serialization") == "xml") {
    std::istringstream ist (str);
    xml_iarchive is (ist);
    is >> BOOST_SERIALIZATION_NVP(m);

  } else {
    std::istringstream ist (str);
    binary_iarchive is (ist);
    is >> BOOST_SERIALIZATION_NVP(m);
  }
  return m;
}

Message* load_message (boost::asio::streambuf& buf) {
  Message* m; 
  if (GET_STR("network.serialization") == "xml") {
    std::istream ist (&buf);
    xml_iarchive is (ist);
    is >> BOOST_SERIALIZATION_NVP(m);
  } else {
    binary_iarchive is (buf);
    is >> BOOST_SERIALIZATION_NVP(m);
  }
  return m;
}

std::string* save_message (Message* m) {
  ostringstream oss;

  if (GET_STR("network.serialization") == "xml") {
    xml_oarchive os (oss);
    os << BOOST_SERIALIZATION_NVP(m);
  } else {
    binary_oarchive os (oss);
    os << BOOST_SERIALIZATION_NVP(m);
  }

  oss.seekp(0, ios::end);
  auto size = oss.tellp();
  oss.seekp(0, ios::beg);

  stringstream ss;
  ss << setfill('0') << setw(16) << size; 
  auto out = new string(ss.str());
  out->append(oss.str());

  return out;
}

void send_message(boost::asio::ip::tcp::socket* socket, 
    eclipse::messages::Message* msg) {
  boost::asio::ip::tcp::no_delay option(true);
  socket->set_option(option);
  string* to_send = save_message(msg);
  socket->send(boost::asio::buffer(*to_send));
  delete to_send;
}


} /* messages */
}
