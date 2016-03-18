#include "common/context.hh"
#include "directory.hh"
#include "../messages/filelist.hh"
#include "../messages/boost_impl.hh"
#include "../messages/factory.hh"
#include <iostream>
#include <dirent.h>
#include <cstdio>
#include <boost/asio.hpp>
#include <iomanip>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

boost::asio::io_service iosvc;

tcp::socket* connect (int index) { 
  tcp::socket* socket = new tcp::socket (iosvc);
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.port_mapreduce");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ index ];

  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);
  socket->connect(*ep);
  delete ep;
  return socket;
}

void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;

  socket->send(boost::asio::buffer(ss.str()));
}

eclipse::messages::Reply* read_reply(tcp::socket* socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  eclipse::messages::Message* m = load_message(recv_msg);
  delete[] body;
  return dynamic_cast<eclipse::messages::Reply*>(m);
}

int main(int argc, char* argv[]) {
  Context con;
  int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  vector<string> nodes = con.settings.get<vector<string>>("network.nodes");
  vector<FileInfo> total;

  for (int net_id = 0; net_id < NUM_SERVERS; net_id++) {
    FormatRequest fr;
    tcp::socket* socket = connect(net_id);
    send_message(socket, &fr);
    auto reply = read_reply(socket);

    if (reply->message != "OK") {
      cerr << "Failed to upload file. Details: " << reply->details << endl;
      delete reply;
      return EXIT_FAILURE;
    } 
    delete reply;

    socket->close();
    delete socket;
  }
  return EXIT_SUCCESS;
}
