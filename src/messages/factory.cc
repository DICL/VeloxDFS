#include "factory.hh"
#include "boost_impl.hh"

#include <sstream>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace boost::asio;
using namespace boost::archive;
using namespace std;

namespace eclipse {
namespace messages {

void load_n (boost::asio::streambuf& data_, function<void(Message*)> f) {
  istream is (&data_);
  xml_iarchive ia (is, no_header);

  while (data_.in_avail() > 1) { //! 1 is the EOF character
    Message* m = nullptr;
    ia >> BOOST_SERIALIZATION_NVP(m);
    f(m);
  }
}

void operator<< (string s, Message* m) {
  s = save_message(m);
}

void operator<< (Message* m , std::string s) {
  m = load_message(s);
}

void operator<< (Message* m, boost::asio::streambuf& data_) {

  auto buf = data_.data();
  std::string s (boost::asio::buffers_begin(buf),
                   boost::asio::buffers_end(buf));

  std::stringstream is(s);
  xml_iarchive ia (is, no_header);

  ia >> BOOST_SERIALIZATION_NVP(m);
}

void operator<< (Histogram& h, Message& m) {
  Boundaries* b = dynamic_cast<Boundaries*>(&m);
  for (int i = 0; i < h.get_numserver(); i++)
    h.set_boundary(i, b->data[i]);
}

Message* load_message (std::string s) {
  Message* m; 
  stringstream ss (s);
  xml_iarchive ia (ss, no_header);

  ia >> BOOST_SERIALIZATION_NVP(m);
  return m;
}

std::string save_message (Message* m) {
  ostringstream ss;
  xml_oarchive ia (ss, no_header);

  ia << BOOST_SERIALIZATION_NVP(m);
  return ss.str();
}

} /* messages */
}
