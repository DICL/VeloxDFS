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
      ifstream myfile (file_name);
      myfile.seekg(1, myfile.end);
      uint64_t file_size = myfile.tellg();
      uint32_t start = 0;
      uint32_t end = start + BLOCK_SIZE - 1;
      uint32_t file_hash_key = h (file_name);
      unsigned int block_seq = 0;
      unsigned int num_blocks = (file_size / BLOCK_SIZE) + 1;

      auto socket = connect(file_hash_key);

      //TODO: remote_metadata_server = lookup(hkey);
      //int remote_metadata_server = 1;
      FileInfo file_info;
      file_info.file_name = file_name;
      file_info.file_hash_key = file_hash_key;
      file_info.file_size = file_size;
      file_info.num_block = num_blocks;
      file_info.replica = con.settings.get<int>("filesystem.replica");

      send_message(socket, &file_info);
      auto reply = read_reply (socket);

      if (reply->message != "OK") {
        cerr << "Failed to upload file. Details: " << reply->details << endl;
        delete reply;
        return EXIT_FAILURE;
      } 
      delete reply;

      while(1)
      {
        if(end < file_size)
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
          BlockInfo block_info;
          bzero(chunk, BLOCK_SIZE);
          myfile.seekg(start, myfile.beg);
          block_info.content.reserve(end-start);
          myfile.read(chunk, end-start);
          block_info.content = chunk;


          uint32_t block_size = end - start;
          start = end + 1;
          end = start + BLOCK_SIZE - 1;

          unsigned int block_hash_key = rand()%NUM_SERVERS;
          //TODO: int remote_server = lookup(block_hash_key);
          //int remote_server = 1;

          block_info.file_name = file_name;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_info.file_name + "_" + to_string(block_seq++);
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
          file_info.num_block = block_seq;
            
          
          send_message(socket, &block_info);
          auto reply = read_reply (socket);

          if (reply->message != "OK") {
            cerr << "Failed to upload file. Details: " << reply->details << endl;
            delete reply;
            return EXIT_FAILURE;
          } 
          delete reply;

          //TODO: remote_metadata_server.update_file_metadata(fileinfo.file_name, file_info);
          //cout << "remote_metadata_server.update_file_metadata(fileinfo.file_name, file_info);" << endl;

          //TODO: remote_metadata_server.insert_block_metadata(blockinfo);
          //cout << "remote_metadata_server.insert_block_metadata(blockinfo);" << endl;

          //TODO: remote_server.send_buff(block_info.block_name, buff);
          //cout << "remote_server.send_buff(block_info.block_name, buff);" << endl;
          //remote_server.send_buff(block_hash_key, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server?

          // TODO: remote node part
        }
        else // last block
        {  
          bzero(chunk, BLOCK_SIZE);
          BlockInfo block_info;
          myfile.seekg(start, myfile.beg);
          block_info.content.reserve(end-start);
          myfile.read(chunk, end-start);
          block_info.content = chunk;

          uint32_t block_size = end - start;

          uint32_t block_hash_key = rand()%NUM_SERVERS;

          // TODO: remote_server = lookup(block_hash_key);
          //cout << "remote_server = lookup(block_hash_key);" << endl;

          block_info.file_name = file_name;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_name + "_" + to_string(block_seq++);
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
          file_info.num_block = block_seq;

          send_message(socket, &block_info);
          auto reply = read_reply (socket);

          if (reply->message != "OK") {
            cerr << "Failed to upload file. Details: " << reply->details << endl;
            delete reply;
            return EXIT_FAILURE;
          } 
          delete reply;
          break;
        }
      }
      socket->close();
      delete socket;
      myfile.close();
    }
    delete[] chunk;
  }
  return 0;
}
