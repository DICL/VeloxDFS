#include "directory.hh"
#include <iomanip>

using namespace std;
using namespace eclipse;

int main(int argc, char* argv[])
{
  Context con;
  //const unsigned int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  vector<string> nodes = con.settings.get<vector<string>>("network.nodes");
  vector<string>::iterator nit = nodes.begin();

  //for test
  const unsigned int NUM_SERVERS = 1;
  
  Directory dir;
  vector<FileInfo> file_list;
  //vector<BlockInfo> block_list;
  
  for(int net_id=0; net_id<NUM_SERVERS; net_id++)
  {
    //TODO: remote_node[net_id].receive_file_metadata(file_list);
    //TODO: remote_node[net_id].receive_block_metadata(file_list); -> maybe not needed

    // remote side
    dir.select_all_file_metadata(file_list);
    //dir.select_all_block_metadata(block_list);

    //TODO: send_file_metadata(net_id, file_list); -> send info to client
    //TODO: send_block_metadata(net_id, block_list); -> send info to client

    cout << *nit << endl;
    nit++;

    // client side
    for(vector<FileInfo>::iterator it=file_list.begin(); it!=file_list.end(); it++)
    {
      cout << setw(5) << it->file_hash_key
      << setw(5) << it->file_size
      << setw(5) << it->num_block
      << setw(5) << it->replica
      << "\t" << it->file_name << endl;
    }
  }
  return 0;
}
