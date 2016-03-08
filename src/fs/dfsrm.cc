#include <iostream>
#include <fstream>
#include <string>
#include "common/hash.hh"
#include "common/context.hh"
#include "fileinfo.hh"
#include "blockinfo.hh"
//#include "directory.hh" metadata save/load

using namespace std;
using namespace eclipse;

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
    string path = con.settings.get<string>("path.scratch");
    for(int i=1; i<argc; i++)
    {
      string file_name = argv[i];
      //uint32_t file_hash_key = h(file_name);

      //TODO: remote_metadata_server = lookup(file_hash_key);

      // TODO: if(!remote_metadata_server.is_exist(file_name))
      if(0)
      {
        cout << "[ERROR]: file " << file_name << " does not exist" << endl;
      }
      else
      {
        FileInfo file_info;
        // TODO: remote_metadata_server.select_file_metadata(file_name, &file_info);
        cout << "remote_metadata_server.select_file_metadata(file_name, &file_info);" << endl;

        // for test
        file_info.num_block = 16;

        for(unsigned int block_seq=0; block_seq<file_info.num_block; block_seq++)
        {
          BlockInfo block_info;
          // TODO: remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info);
          //cout << "remote_metadata_server.select_block_metadata(file_name, block_seq, &block_info)" << endl;
          
          // TODO: remote_block_server.lookup(block_info.block_hash_key);
          //cout << "remote_block_server.lookup(block_info.block_hash_key)" << endl;

          // for test
          block_info.block_name = file_name + "_" + to_string(block_seq);


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
  }
  return 0;
}
