#include "../common/hash.hh"
#include "../common/context.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "directory.hh"

#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <boost/asio.hpp>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

boost::asio::io_service iosvc;

void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;

  socket->send(boost::asio::buffer(ss.str()));
}

eclipse::messages::CacheInfo* read_reply(tcp::socket* socket) {
  char header[16];
  socket->receive(boost::asio::buffer(header));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body);
  eclipse::messages::Message* m = load_message(recv_msg);
  return dynamic_cast<eclipse::messages::CacheInfo*>(m);
}

int main(int argc, char *argv[])
{
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.port_mapreduce");
  vec_str nodes = setted.get<vec_str> ("network.nodes");


  std::vector<tcp::socket*> sockets;
  for (auto node: nodes) {
    sockets.push_back(new tcp::socket(iosvc));
    tcp::resolver resolver (iosvc);
    tcp::resolver::query query (node, to_string(port));
    tcp::resolver::iterator it (resolver.resolve(query));
    auto ep = new tcp::endpoint (*it);
    sockets.back()->connect(*ep);
  }

  CacheInfo msg;

  for (auto socket : sockets) {
    send_message(socket, &msg);
  }

  int i = 0;
  for (auto socket : sockets) {
    auto reply = read_reply(socket);
    cout << "Server: " << i++ << endl;
    for (auto key : reply->keys) {
      cout << "- " << key << endl;
    }
  }

  for (auto socket : sockets) {
    socket->close();
    delete socket;
  }
}
