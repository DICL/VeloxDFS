#include "DHTclient.hh"

#include "../common/definitions.hh"
#include "../messages/boost_impl.hh"

#include <boost/lexical_cast.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/exception/all.hpp>
#include <thread>
#include <functional>
#include <sstream>

using boost::lexical_cast;
using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace boost::archive;
using namespace std;
using namespace std::placeholders;
using namespace network;

// constructor & destructor {{{
DHTclient::DHTclient(string& master, int nservers) : 
  inbound_header_ (DHTdefinitions::header_length)
{ 
  Settings setted = Settings().load();

  int port = setted.get<int> ("network.port_dht");

  tcp::resolver resolver (io_service);
  tcp::resolver::query query (master, lexical_cast<string>(port));
  endpoint_iterator.reset (new tcp::resolver::iterator (resolver.resolve(query)));

  histogram.reset (new Histogram (nservers, NUMBIN));
  die_thread = false;
}

DHTclient::~DHTclient() {
  die_thread = true;
  if (detached) detached->join();
  histogram.reset();
}
// }}}
// connect {{{
bool DHTclient::connect () {
    socket.reset (new tcp::socket (io_service));
    boost::asio::connect(*socket, *endpoint_iterator);
  return true;
}
// }}}
// close {{{
void DHTclient::close() { }
// }}}
// index_of {{{
int DHTclient::index_of (std::string input) const {
  int i = hash_ruby (input.c_str());
  return histogram->get_index(i);
}

int DHTclient::index_of (uint64_t i) const {
  return histogram->get_index(i);
}
// }}}
// detach {{{
void DHTclient::detach() {
  async_read (*socket, inbound_data_, bind (&DHTclient::on_read_body, this, _1, _2));

  detached.reset (new thread([&] () { io_service.run(); }));
}
// }}}
// on_read_body {{{
void DHTclient::on_read_body (boost::system::error_code ec, size_t bytes_received) {
  using namespace boost::serialization;
  if (die_thread) return;

  istream is (&inbound_data_);
  xml_iarchive ia (is, no_header);

  Boundaries* b = nullptr;

  while (inbound_data_.in_avail() > 1) { //! 1 is the EOF character
    Message* m = nullptr;
    ia >> BOOST_SERIALIZATION_NVP(m);
    b = dynamic_cast<Boundaries*>(m);
  }

  for (int i = 0; i < histogram->get_numserver(); i++)
    histogram->set_boundary (i, b->data[i]);

  delete b;
  if (!ec) { //If it is not end-of-file
    async_read (*socket, inbound_data_, bind (&DHTclient::on_read_body, this, _1, _2));

  } else {
    cout << ec.message() << endl;
  }
}
// }}}
