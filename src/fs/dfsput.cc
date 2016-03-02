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

int main(int argc, char* argv[])
{
  Context con;
  Directory dir;
  if(argc < 2)
  {
    cout << "usage: dfsput file_name1 file_name2 ..." << endl;
    return -1;
  }
  else
  {
    
    uint32_t BLOCK_SIZE = con.settings.get<int>("filesystem.block");
    uint32_t NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
    string path = con.settings.get<string>("path.scratch");
    //file_info.replica = con.settings.get<int>("filesystem.replica");
    for(int i=1; i<argc; i++)
    {
      char *buff = new char[BLOCK_SIZE];
      bzero(buff, BLOCK_SIZE);
      ifstream myfile;
      string file_name = argv[i];
      myfile.open(file_name);
      myfile.seekg(0, myfile.end);
      uint64_t file_size = myfile.tellg();
      unsigned int block_seq = 0;
      uint32_t start = 0;
      uint32_t end = start + BLOCK_SIZE - 1;
      uint32_t file_hash_key = h (file_name);

      auto socket = connect(file_hash_key);

      //TODO: remote_metadata_server = lookup(hkey);
      int remote_metadata_server = 1;

      uint32_t file_id;
      while(1)
      {
        file_id = h(file_name);

        // TODO: if(!remote_metadata_server.is_exist(file_id))
        if(1)

        {
          break;
        }
      }
      FileInfo file_info;
      file_info.file_id = file_id;
      file_info.file_name = file_name;
      file_info.file_hash_key = file_hash_key;
      file_info.file_size = file_size;
      file_info.num_block = block_seq;

      send_message(socket, &file_info);

      while(1)
      {
        
        if(end < file_size){
          myfile.seekg(start+BLOCK_SIZE-1, myfile.beg);

          while(1)
          {
            if(myfile.peek() =='\n') break;
            else
            {
              myfile.seekg(-1, myfile.cur);
              end--;
            }
          }
          myfile.seekg(start, myfile.beg);
          myfile.read(buff, end-start);
          uint32_t block_size = end - start;
          start = end + 1;
          end = start + BLOCK_SIZE - 1;

          unsigned int block_hash_key = rand()%NUM_SERVERS;

          //TODO: int remote_server = lookup(block_hash_key);
          //int remote_server = 1;

          BlockInfo block_info;
          block_info.file_id = file_id;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_info.file_name + "_" + to_string(block_seq++);
          block_info.block_size = block_size;
          block_info.is_inter = 0;
          //block_info.node = remote_server.ip_address;
          //l_server = lookup((block_hash_key-1+NUM_SERVERS)%NUM_SERVERS);
          //r_server = lookup((block_hash_key+1+NUM_SERVERS)%NUM_SERVERS);
          //block_info.l_node = l_server.ip_address;
          //block_info.r_node = r_server.ip_address;
          block_info.is_commit = 1;
          file_info.num_block = block_seq;

          //TODO: remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);
          //cout << "remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);" << endl;

          //TODO: remote_metadata_server.insert_block_metadata(blockinfo);
          //cout << "remote_metadata_server.insert_block_metadata(blockinfo);" << endl;

          //TODO: remote_server.send_buff(block_info.block_name, buff);
          //cout << "remote_server.send_buff(block_info.block_name, buff);" << endl;
          //remote_server.send_buff(block_hash_key, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server?

          // TODO: remote node part
          ofstream block;
          block.open(path + "/" + block_info.block_name);
          block << buff;
          block.close();
        }
        else{  // last block
          myfile.seekg(start, myfile.beg);
          myfile.read(buff, file_size-start);
          uint32_t block_size = end - start;
          buff[file_size-start-1] = 0;

          uint32_t block_hash_key = rand()%NUM_SERVERS;

          // TODO: remote_server = lookup(block_hash_key);
          //cout << "remote_server = lookup(block_hash_key);" << endl;

          BlockInfo block_info;
          block_info.file_id = file_id;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_name + "_" + to_string(block_seq++);
          block_info.block_size = block_size;
          block_info.is_inter = 0;
          //block_info.node = remote_server.ip_address;
          //l_server = lookup(block_hash_key-1);
          //r_server = lookup(block_hash_key+1);
          //block_info.l_node = l_server.ip_address;
          //block_info.r_node = r_server.ip_address;
          block_info.is_commit = 1;
          file_info.num_block = block_seq;

          // TODO: remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);
          //cout << "remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);" << endl;

          // TODO: remote_metadata_server.insert_block_metadata(blockinfo);
          //cout << "remote_metadata_server.insert_block_metadata(blockinfo);" << endl;

          // TODO: remote_server.send_buff(block_info.block_name, buff);
          //cout << "remote_server.send_buff(block_info.block_name, buff);" << endl;
          //remote_server.send_buff(block_hash_key, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server;

          // remote node part
          ofstream block;
          block.open(path + "/" + block_info.block_name);
          block << buff;
          block.close();
          break;
        }
      }
      socket->close();
      myfile.close();
      delete[] buff;
    }

  }
  return 0;
}
