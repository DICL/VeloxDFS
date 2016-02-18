#pragma once
#include <string>
#include <../network/syncnetwork.hh>
#include <../network/client.hh>

namespace eclipse {

class DataSet {
  public:
    DataSet& map(std::string);
    static Dataset& open (std::string);
  
  private:
    DataSet (int);
    int id;
    void connect();

    network::SyncNetwork<Client> network;
};

}
