#include "../common/hash.hh"
#include "../common/context.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "directory.hh"

#include <iostream>
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

int main(int argc, char* argv[])
{
  Context con;

  if(argc < 2)
  {
    cout << "usage: dfsput file_name1 file_name2 ..." << endl;
    return -1;
  }
  else
  {
    uint32_t BLOCK_SIZE = con.settings.get<int>("filesystem.block");
    uint32_t NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
    char* chunk = new char[BLOCK_SIZE];
    for(int i=1; i<argc; i++)
    {
      string file_name = argv[i];
      ifstream myfile (argv[i]);
      uint64_t start = 0;
      uint64_t end = start + BLOCK_SIZE - 1;
      uint32_t block_size = 0;
      unsigned int block_seq = 0;

      FileInfo file_info;
      file_info.file_name = file_name;
      file_info.file_hash_key = h(file_name);
      file_info.replica = con.settings.get<int>("filesystem.replica");
      myfile.seekg(0, myfile.end);
      file_info.file_size = myfile.tellg();
      BlockInfo block_info;

      auto socket = connect(file_info.file_hash_key);

      while(1)
      {
        if(end < file_info.file_size)
        {
          myfile.seekg(start+BLOCK_SIZE-1, myfile.beg);
          while(1)
          {
            if(myfile.peek() =='\n')
            {
              end++;
              break;
            }
            else
            {
              myfile.seekg(-1, myfile.cur);
              end--;
            }
          }
        }
        block_size = (uint32_t) end - start;
        bzero(chunk, BLOCK_SIZE);
        myfile.seekg(start, myfile.beg);
        block_info.content.reserve(block_size);
        myfile.read(chunk, block_size);
        block_info.content = chunk;

        block_info.block_name = file_name + "_" + to_string(block_seq++);
        block_info.file_name = file_name;
        block_info.block_seq = block_seq;
        block_info.block_hash_key = (unsigned int) rand()%NUM_SERVERS;
        block_info.block_size = block_size;
        block_info.is_inter = 0;
        block_info.node = "1.1.1.1";
        block_info.l_node = "1.1.1.0";
        block_info.r_node = "1.1.1.2";
        //block_info.node = remote_server.ip_address;
        //Node l_node = lookup((block_hash_key-1+NUM_SERVERS)%NUM_SERVERS);
        //Node r_node = lookup((block_hash_key+1+NUM_SERVERS)%NUM_SERVERS);
        //block_info.l_node = l_node.ip_address;
        //block_info.r_node = r_node.ip_address;

        send_message(socket, &block_info);
        auto reply = read_reply (socket);

        if (reply->message != "OK") {
          cerr << "Failed to upload file. Details: " << reply->details << endl;
          delete reply;
          return EXIT_FAILURE;
        } 
        delete reply;

        if(end >= file_info.file_size)
        {
          break;
        }
        start = end + 1;
        end = start + BLOCK_SIZE - 1;
      }

      file_info.num_block = block_seq;

      send_message(socket, &file_info);
      auto reply = read_reply (socket);

      if (reply->message != "OK") {
        cerr << "Failed to upload file. Details: " << reply->details << endl;
        delete reply;
        return EXIT_FAILURE;
      } 
      delete reply;

      socket->close();
      delete socket;
      myfile.close();
    }
    delete[] chunk;
  }
  return 0;
}
