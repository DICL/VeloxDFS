#include <network/centralizedtopology.hh>
#include <common/settings.hh>
#include <string>

using namespace eclipse::network;
using namespace std;
using eclipse::network::vec_str;

int main (int argc, char ** argv) {
  string input = argv[1];
  Settings setted (input);
  setted.load();

  vec_str net = setted.get<vec_str> ("network.nodes");
  string master = setted.get<string> ("network.master");
  int port = setted.get<int> ("network.port_mr");
  int id = setted.get<int> ("id");

  string logname = setted.get<string> ("log.name");
  string logtype = setted.get<string> ("log.type");

  Logger* lg = Logger::connect(logname, logtype);

  boost::asio::io_service io;

  CentralizedTopology ct (io, lg, master, net, port, id);

  ct.establish();

  io.run();
  return 0;
}
