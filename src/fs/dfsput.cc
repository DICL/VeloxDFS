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
    const uint32_t BUF_SIZE = con.settings.get<int>("size.buffer");
    const uint32_t BLOCK_SIZE = con.settings.get<int>("size.block");
    string path = con.settings.get<string>("path.scratch");
    char buff[BUF_SIZE] = {0};
    ifstream myfile;
    for(unsigned int i=0; i<argc; i++)
    {
      string file_name = path + "/" + argv[i];
      myfile.open(file_name);
      uint32_t hkey = hash_ruby(file_name);
      // PSEUDO CODE
      remote_metadata_server = lookup(hkey);
      remote_metadata_server.update_file_metadata(filename, filesize, ...);
      // this function should call FileIO.open_file() in remote metadata server;

      myfile.seekg(0, myfile.end);
      int file_size = myfile.tellg();
      int block_seq = 0;
      int start = 0;
      int end = start + BLOCK_SIZE - 1;
      while(1){
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
          start = end + 1;
          end = start + BLOCK_SIZE - 1;

          //cout << "last block" << "-----" << endl; 
          cout << buff << endl << "-----" << endl;
          hkey = rand()%NUM_SERVERS;
          remote_server = lookup(hkey);
          remote_server.update_directory(fileinfo, blockinfo);
          remote_server.send_buff(hkey, buff);
          // this function should call FileIO.insert_block(_metadata) in remote metadata server;

          // remote node part
          ofstream block;
          string block_name = file_name + "_" + to_string(i);
          block.open(block_name);
          block << buff ;
          block.close();
        }
        else{  // last block
          myfile.seekg(start, myfile.beg);
          myfile.read(buff, file_size-start);
          buff[file_size-start-1] = 0;

          //cout << "last block" << "-----" << endl; 
          cout << buff << endl << "-----" << endl;
          hkey = rand()%NUM_SERVERS;
          remote_server = lookup(hkey);
          remote_server.update_directory(fileinfo, blockinfo);
          remote_server.send_buff(hkey, buff);
          
          // remote node part
          ofstream block;
          string block_name = file_name + "_" + to_string(i);
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
