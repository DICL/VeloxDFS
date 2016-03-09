#include "directory.hh"

using namespace eclipse;

int main(int argc, char* argv[])
{
  const unsigned int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  
  for(int net_id=0; net_id<NUM_SERVERS; net_id++)
  {
    string list
    //TODO: remote_node[net_id].list_file_metadata(list);
    //TODO: remote_node[net_id].list_block_metadata(list); -> maybe not needed

    // for test
    list = "doc1.txt doc2.txt doc3.txt doc4.txt";

    while(!list.end)
    {
      //TODO: parsing_message(list);

      //TODO: cout << list;
    }
  
    // NOT FINISHED...
  }
  return 0;
}
