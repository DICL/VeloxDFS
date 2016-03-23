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
    cout << "usage: dfsrm file_name1 file_name2 ..." << endl;
    return -1;
  }
  else
  {
    //uint32_t BLOCK_SIZE = con.settings.get<int>("filesystem.block");
    //uint32_t NUMSERVERS = con.settings.get<vector<string>>(
        //"network.nodes").size();
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
        auto *tmp_socket = connect(h(block_name.c_str()));
        BlockDel bd;
        bd.block_name = block_name;
        bd.file_name = file_name;
        bd.block_seq = block_seq++;
        send_message(tmp_socket, &bd);
        auto msg = read_reply(tmp_socket);
        if (msg->message != "OK") {
          cerr << "[ERROR]: block " << block_name << "doesn't exist" << endl;
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
        cerr << "[ERROR]: file " << file_name << " does not exist" << endl;
        delete reply;
        return EXIT_FAILURE;
      }
      delete reply;
      socket->close();
      delete socket;
/*
      if (reply->message != "OK") {
      {
        cerr << "[ERROR]: file " << file_name << " does not exist" << endl;
        return EXIT_FAILURE;
      }
      else
      {
        FileRequest file_request;
        send_message(scoket, &file_reqeust);
        FileDescription file_description;
        file_description = read_fd(scoket);
        //cout << "remote_metadata_server.select_file_metadata(file_name, &file_info);" << endl;

        file_del.num_block = file_description.nodes.size();

        for(unsigned int block_seq=0; block_seq<file_del.num_block; block_seq++)
        {
          // TODO: remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info);
          //cout << "remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info)" << endl;
          
          // TODO: remote_block_server.lookup(block_info.block_hash_key);
          //cout << "remote_block_server.lookup(block_info.block_hash_key)" << endl;

          // for test
          //block_info.block_name = file_name + "_" + to_string(block_seq);
          BlockRequest


          string rmblock = path + "/" + block_info.block_name;

          // TODO: remote_block_server.remove(rmblock.c_str());
          //cout << "remote_block_server.remove(rmblock.c_str())" << endl;

          // TODO: remote_metadata_server.delete_block_metadata(file_name, block_seq, &block_info);
          //cout << "remote_metadata_server.delete_block_metadata(file_name, block_seq, &block_info)" << endl;


          // remote server side
          //cout << rmblock.c_str() << endl;
          remove(rmblock.c_str());
        }
      }
    }
*/
    }
  }
  return 0;
}
