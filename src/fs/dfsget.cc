#include "common/hash.hh"
#include "common/context.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "../messages/filerequest.hh"
#include "../messages/blockrequest.hh"
#include "../messages/factory.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;

using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

boost::asio::io_service iosvc;

tcp::socket* connect (int hash_value) { 
  tcp::socket* socket = new tcp::socket (iosvc);
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.port_mapreduce");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ hash_value % nodes.size() ];

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

eclipse::messages::FileDescription* read_reply(tcp::socket* socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  eclipse::messages::Message* m = load_message(recv_msg);
  delete[] body;
  return dynamic_cast<eclipse::messages::FileDescription*>(m);
}

eclipse::messages::BlockInfo* read_block(tcp::socket* socket) {
  char header[17] = {0};
  header[16] = '\0';
  socket->receive(boost::asio::buffer(header, 16));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body, size_of_msg);
  eclipse::messages::Message* m = load_message(recv_msg);
  delete[] body;
  return dynamic_cast<eclipse::messages::BlockInfo*>(m);
}

int main(int argc, char* argv[]) {
  Context con;
  if (argc < 2) {
    cout << "usage: dfsget file_name1 file_name2 ..." << endl;
    return EXIT_FAILURE;

  } else {
    string path = con.settings.get<string>("path.scratch");

    for (int i=1; i<argc; i++) {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      tcp::socket* socket = connect (file_hash_key);
      FileRequest fr;
      fr.file_name = file_name;
      
      send_message (socket, &fr);
      auto fd = read_reply (socket);

      ofstream f (file_name);
      for (auto block_name : fd->nodes) {
        auto* tmp_socket = connect(h(block_name.c_str()));
        BlockRequest br;
        br.block_name = block_name; 
        send_message(tmp_socket, &br);
        auto msg = read_block(tmp_socket);
        f << msg->content;
        tmp_socket->close();
        delete tmp_socket;
        delete msg;
      }

      cout << file_name << " downloaded" << endl;
      socket->close(); 
      f.close();
      delete socket;
      delete fd;
    }
  }
  return EXIT_SUCCESS;
}
