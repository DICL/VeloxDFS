#pragma once

#include "DHTdefinitions.hh"
#include "../common/definitions.hh"
#include "../common/histogram.hh"
#include "../nodes/ipush.hh"

#include <memory>
#include <boost/asio.hpp>

class DHTserver: public Nodes::iPush {
  typedef boost::asio::ip::tcp::iostream _ios;
  public:
    DHTserver (int);
    ~DHTserver ();

    void listen () override;
    void accept () override;
    void close () override;

    void count_query (uint64_t);

  private:
    int port, num_servers;
    std::unique_ptr<Histogram> histogram;

    std::map<int, std::unique_ptr<_ios> > network_table;
    boost::asio::io_service io_service;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
};
