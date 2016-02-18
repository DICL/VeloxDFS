#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;

int main()
{

/*
  // open file to upload

  hk = hash(file_name);

  // connect to a server whose hash key range includes hk
  S = hash_key_range_lookup(hk);
  S.open_file(file_name, O_WRONLY);
*/
  //char buff[251] = {0};
  char buff[251];
  memset(buff, 0, 251);

  ifstream myfile;
  myfile.open ("example.txt");
  //myfile << "Writing this to a file.\n";


  int hkey = hash("example.txt");
  remote_metadata_server = lookup(hkey);

  remote_metadata_server.update_file_metadata(filename, filesize, ...);
  // this function should call FileIO.open_file() in remote metadata server;

  myfile.seekg(0,myfile.end);
  int file_size=myfile.tellg();
  int block_seq =0;
  //int CHUNK_SIZE = 64*1024*1024;
  int CHUNK_SIZE = 250;
  int start = 0;
  int end = start + CHUNK_SIZE;
  while(1){
    if(end < file_size){
        myfile.seekg(start+CHUNK_SIZE, myfile.beg);
    
        while(1){
          if(myfile.peek() =='\n') break;
          else {
            myfile.seekg(-1, myfile.cur);
            end--;
          }
        }
        myfile.seekg(start, myfile.beg);
        myfile.read(buff, end-start);
        start = end+1;
        end = start + CHUNK_SIZE;

        //cout << "partitioned block" << "-----" << endl; 
        //cout << buff << endl << "-----" << endl;
        ofstream chunkfile;
        chunkfile.open("chunk.txt");
        chunkfile << buff ;
        chunkfile.close();

        int hkey = rand()%NUM_SERVERS;
        remote_server = lookup(hkey);

        remote_server.update_directory(fileinfo, blockinfo);
        // this function should call FileIO.insert_block(_metadata) in remote metadata server;

        sprintf(command, "scp chunk.txt %s\n", remote_server.host_name);
        system(command);
    }
    else{  // last block
        myfile.seekg(start, myfile.beg);
        myfile.read(buff, file_size-start);
        buff[file_size-start-1] = 0;

        //cout << "last block" << "-----" << endl; 
        //cout << buff << endl << "-----" << endl;
        ofstream chunkfile;
        chunkfile.open("chunk.txt");
        chunkfile << buff ;
        chunkfile.close();

        int hkey = rand()%NUM_SERVERS;
        remote_server = lookup(hkey);
        remote_server.update_directory(fileinfo, blockinfo);
        sprintf(command, "scp chunk.txt %s\n", remote_server.host_name);
        system(command);

        break;
    }
  }

  myfile.close();

}
