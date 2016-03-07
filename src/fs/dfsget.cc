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
  return socket;
}

void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;

  socket->send(boost::asio::buffer(ss.str()));
}

eclipse::messages::FileDescription* read_reply(tcp::socket* socket) {
  char header[16];
  socket->receive(boost::asio::buffer(header));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body);
  eclipse::messages::Message* m = load_message(recv_msg);
  return dynamic_cast<eclipse::messages::FileDescription*>(m);
}

int main(int argc, char* argv[])
{
  Context con;
  if(argc < 2)
  {
    cout << "usage: dfsget file_name1 file_name2 ..." << endl;
    return -1;
  }
  else
  {
    string path = con.settings.get<string>("path.scratch");
    for(int i=1; i<argc; i++)
    {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      tcp::socket* socket = connect (file_hash_key);
      FileRequest fr;
      fr.file_name = file_name;
      
      send_message (socket, &fr);
      auto fd = read_reply (socket);

      cout << "Got " << fd->file_name << endl;

      for (auto block_name : fd->nodes) {
        auto* tmp_socket = connect(h(block_name.c_str()));
        BlockRequest br;
        br.block_name = block_name; 
        send_message(tmp_socket, &br);
        tmp_socket->close();
      }

/*
      //TODO: remote_metadata_server = lookup(file_hash_key);
      //int remote_metadata_server = 1;

      // TODO: if(!remote_metadata_server.is_exist(file_name))
      if(0)
      {
        cout << "[ERROR]: file " << file_name << " does not exist" << endl;
      }
      else
      {
        FileInfo file_info;
        // TODO: remote_metadata_server.select_file_metadata(file_name, &file_info);
        //cout << "remote_metadata_server.select_file_metadata(file_name, &file_info);" << endl;

        // for test
        file_info.num_block = 16;

        for(unsigned int block_seq=0; block_seq<file_info.num_block; block_seq++)
        {
          BlockInfo block_info;
          // TODO: remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info)
          //cout << "remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info)" << endl;

          // TODO: remote_metadata_server.open(block_info.block_name);
          //cout << "remote_metadata_server.open(block_info.block_name)" << endl;

          // for test
          block_info.block_name = file_name + "_" + to_string(block_seq);

          // remote server side
          ifstream input_file;
          input_file.open(path + "/" + block_info.block_name, ifstream::in);
          string buff;
          buff.assign((istreambuf_iterator<char>(input_file)), (istreambuf_iterator<char>()));
          input_file.close();

          // TODO: remote_metadata_server.receive_buff(buff);
          //cout << "remote_metadata_server.receive_buff(buff)" << endl;

          ofstream output_file;
          output_file.open(file_name, ostream::out | ofstream::app);
          output_file << buff;
          output_file.close();
        }
      }
      */
    }
  }
  return 0;
}
