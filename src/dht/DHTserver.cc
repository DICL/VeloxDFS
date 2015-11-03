#include "DHTserver.hh"
#include "../common/settings.hh"
#include "../common/hash.hh"
#include "../messages/boost_impl.hh"

#include <boost/asio.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/range/adaptor/map.hpp>
#include <sstream>

using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace boost::adaptors;
using namespace boost::archive;
using namespace network;
using namespace std;

Message* extract_message (Histogram& h) {
  std::vector<uint64_t> vec;
  for (int i = 0; i < h.get_numserver(); ++i)
    vec.push_back (h.get_boundary(i));

  return new network::Boundaries(vec);
}

// constructor & destructor {{{
DHTserver::DHTserver (int nservers) : num_servers (nservers) {
  histogram.reset (new Histogram (nservers, NUMBIN));

  Settings setted = Settings().load();
  port = setted.get<int> ("network.port_dht");
}

DHTserver::~DHTserver () { }
// }}}
// listen {{{
void DHTserver::listen () {
  acceptor.reset(new tcp::acceptor (io_service, tcp::endpoint (tcp::v4(), port)));
}
// }}}
// accept {{{
void DHTserver::accept () {
  for (int i = 0; i < num_servers; ++i) {
    auto stream = new tcp::iostream ();
    acceptor->accept (*stream->rdbuf());

    network_table[i] = unique_ptr<_ios>(stream);
  }
}
// }}}
// count_query {{{
void DHTserver::count_query (uint64_t i) {
  using namespace DHTdefinitions;
  histogram->count_query(i);    
  histogram->updateboundary();

  Message* b = extract_message (*histogram);

  stringstream ss;
  xml_oarchive archive (ss, no_header);
  archive << BOOST_SERIALIZATION_NVP(b);

  for (auto& stream_ : network_table | map_values) {
    string outbound = ss.str();
    *stream_ << outbound;
  }
  delete b;
}
// }}}
// close {{{
void DHTserver::close () {

}
// }}}
