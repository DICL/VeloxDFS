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
    cout << "usage: dfsget file_name1 file_name2 ..." << endl;
    return -1;
  }
  else
  {
    string path = con.settings.get<string>("path.scratch");
    for(int i=1; i<argc; i++)
    {
      string file_name = argv[i];
      //uint32_t file_hash_key = h(file_name);
      //uint32_t file_id = file_hash_key;

      //TODO: remote_metadata_server = lookup(file_hash_key);
      //int remote_metadata_server = 1;

      // TODO: if(!remote_metadata_server.is_exist(file_id))
      if(0)
      {
        cout << "[ERROR]: file " << file_name << " does not exist" << endl;
      }
      else
      {
        FileInfo file_info;
        // TODO: remote_metadata_server.select_file_metadata(file_id, &file_info);
        //cout << "remote_metadata_server.select_file_metadata(file_id, &file_info);" << endl;

        // for test
        file_info.num_block = 16;

        for(unsigned int block_seq=0; block_seq<file_info.num_block; block_seq++)
        {
          BlockInfo block_info;
          // TODO: remote_metadata_server.select_block_metadata(file_id, block_seq, &block_info)
          //cout << "remote_metadata_server.select_block_metadata(file_id, block_seq, &block_info)" << endl;

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
    }
  }
  return 0;
}
