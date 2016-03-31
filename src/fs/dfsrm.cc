#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include "../common/hash.hh"
#include "../common/context.hh"
#include "../messages/filedel.hh"
#include "../messages/blockdel.hh"
#include "../messages/filedescription.hh"
#include "../messages/filerequest.hh"
#include "../messages/factory.hh"
#include "../messages/reply.hh"
#include "directory.hh"
#include <boost/asio.hpp>

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

eclipse::messages::FileDescription* read_fd(tcp::socket* socket) {
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

int main(int argc, char* argv[])
{
  Context con;
  if(argc < 2)
  {
    cout << "[INFO] dfsrm file_1 file_2 ..." << endl;
    return -1;
  }
  else
  {
    uint32_t NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
    Histogram boundaries(NUM_SERVERS, 0);
    boundaries.initialize();

    string path = con.settings.get<string>("path.scratch");
    for(int i=1; i<argc; i++)
    {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      auto socket = connect(file_hash_key);
      FileRequest fr;
      fr.file_name = file_name;

      send_message(socket, &fr);
      auto fd = read_fd(socket);
      socket->close();
      delete socket;

      unsigned int block_seq = 0;
      for (auto block_name : fd->blocks) {
        auto *tmp_socket = connect(boundaries.get_index(fd->hash_keys[block_seq]));
        BlockDel bd;
        bd.block_name = block_name;
        bd.file_name = file_name;
        bd.block_seq = block_seq++;
        send_message(tmp_socket, &bd);
        auto msg = read_reply(tmp_socket);
        if (msg->message != "OK") {
          cerr << "[ERR] " << block_name << "doesn't exist." << endl;
          delete msg;
          return EXIT_FAILURE;
        }
        delete msg;

        tmp_socket->close();
        delete tmp_socket;
      }
      delete fd;

      FileDel file_del;
      file_del.file_name = file_name;
      socket = connect(file_hash_key);
      send_message(socket, &file_del);
      auto reply = read_reply(socket);
      if (reply->message != "OK") {
        cerr << "[ERR] " << file_name << " doesn't exist." << endl;
        delete reply;
        return EXIT_FAILURE;
      }
      delete reply;
      socket->close();
      delete socket;
      cout << "[INFO] " << file_name << " is removed." << endl;
    }
    return 0;
  }
}
