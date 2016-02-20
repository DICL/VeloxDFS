#include <iostream>
#include <string.h>
#include <fstream>
#include <string>
#include "../common/hash.hh"
#include "../common/context.hh"
#include "../directory.hh"

using namespace std;

int main(int argc, char* arvg[])
{
  Context con;
  if(argc < 2)
  {
    con.logger->info("[ERROR]usage: dfsput file_name1 file_name2 ...\n");
  }
  else
  {
    const uint32_t BUF_SIZE = con.settings.get<int>("filesystem.buffer");
    const uint32_t BLOCK_SIZE = con.settings.get<int>("filesystem.block");
    string path = con.settings.get<string>("path.scratch");
    file_info.replica = con.settings.get<int>("filesystem.replica");
    for(unsigned int i=0; i<argc; i++)
    {
      char buff[BUF_SIZE] = {0};
      ifstream myfile;
      string file_name = path + "/" + argv[i];
      myfile.open(file_name);
      myfile.seekg(0, myfile.end);
      uint32_t file_size = myfile.tellg();
      unsigned int block_seq = 0;
      uint32_t start = 0;
      uint32_t end = start + BLOCK_SIZE - 1;
      uint32_t file_hash_key = hash_ruby(file_name);
      remote_metadata_server = lookup(hkey);
      uint32_t file_id;
      while(1)
      {
        file_id = rand();
        if(!remote_metadata_server.is_exist(file_id)
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

      // PSEUDO CODE
      remote_metadata_server.insert_file_metadata(file_info);
      // this function should call FileIO.open_file() in remote metadata server;
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

          //cout << "last block" << "-----" << endl; 
          //cout << buff << endl << "-----" << endl;
          cout << buff << endl ;
          unsigned int block_hash_key = rand()%NUM_SERVERS;
          remote_server = lookup(block_hash_key);

          BlockInfo block_info;
          block_info.file_id = file_id;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_name + "_" + block_seq++;
          block_info.block_size = block_size;
          block_info.is_inter = 0;
          block_info.node = remote_server.ip_address;
          l_server = lookup((block_hash_key-1+NUM_SERVERS)%NUM_SERVERS);
          r_server = lookup((block_hash_key+1+NUM_SERVERS)%NUM_SERVERS);
          block_info.l_node = l_server.ip_address;
          block_info.r_node = r_server.ip_address;
          block_info.commit = 1;
          file_info.num_block = block_seq;

          remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);
          remote_metadata_server.insert_block_metadata(blockinfo);
          remote_server.send_buff(block_info.block_name, buff);
          //remote_server.send_buff(block_hash_key, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server;

          // remote node part
          ofstream block;
          block.open(block_name);
          block << buff ;
          block.close();
        }
        else{  // last block
          myfile.seekg(start, myfile.beg);
          myfile.read(buff, file_size-start);
          uint32_t block_size = end - start;
          buff[file_size-start-1] = 0;

          //cout << "last block" << "-----" << endl; 
          cout << buff << endl;
          uint32_t block_hash_key = rand()%NUM_SERVERS;
          remote_server = lookup(block_hash_key);

          BlockInfo block_info;
          block_info.file_id = file_id;
          block_info.block_seq = block_seq;
          block_info.block_hash_key = block_hash_key;
          block_info.block_name = file_name + "_" + block_seq++;
          block_info.block_size = block_size;
          block_info.is_inter = 0;
          block_info.node = remote_server.ip_address;
          l_server = lookup(block_hash_key-1);
          r_server = lookup(block_hash_key+1);
          block_info.l_node = l_server.ip_address;
          block_info.r_node = r_server.ip_address;
          block_info.commit = 1;
          file_info.num_block = block_seq;

          remote_metadata_server.update_file_metadata(fileinfo.file_id, file_info);
          remote_metadata_server.insert_block_metadata(blockinfo);
          remote_server.send_buff(block_info.block_name, buff);
          //remote_server.send_buff(block_hash_key, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server;

          // remote node part
          ofstream block;
          block.open(block_name);
          block << buff ;
          block.close();
          break;
        }
      }
      myfile.close();
    }

  }
}
