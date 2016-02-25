#include "dataset.hh"
#include "../messages/boost_impl.hh"
#include "../common/ecfs.hh"
#include "../common/hash.hh"
#include <vector>

using namespace eclipse;
using namespace std;
using vec_str = std::vector<std::string>;

DataSet& DataSet::open (std::string in) {
  uint32_t id_ = h(in);
  auto data = new DataSet(id_);
  return *data;
}

DataSet::DataSet (uint32_t id_) : 
  id (id_), 
  socket (iosvc) 
{
  find_local_master();
  auto  ep = find_local_master();
  socket.connect(*ep);
}

tcp::endpoint* DataSet::find_local_master() {
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.port_mapreduce");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ id % nodes.size() ];

  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);

  return ep;
}

DataSet& DataSet::map (std::string func) {

  return *(new DataSet(2131231));

}
