#include "factory.hh"
#include "boost_impl.hh"

#include <sstream>
#include <boost/asio/streambuf.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace boost::asio;
using namespace boost::archive;
using namespace std;

namespace network {

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
  ostringstream ss;
  xml_oarchive ia (ss, no_header);

  ia << BOOST_SERIALIZATION_NVP(m);
  s = ss.str();
}

void operator<< (Message* m, boost::asio::streambuf& data_) {
  istream is (&data_);
  xml_iarchive ia (is, no_header);

  ia >> BOOST_SERIALIZATION_NVP(m);
}

void operator<< (Histogram& h, Message& m) {
  Boundaries* b = dynamic_cast<Boundaries*>(&m);
  for (int i = 0; i < h.get_numserver(); i++)
    h.set_boundary(i, b->data[i]);
}

} /* network */
