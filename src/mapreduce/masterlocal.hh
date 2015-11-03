#pragma once

#include "nodelocal.hh"
#include "../common/definitions.hh"
#include "../common/histogram.hh"
#include "ipush.hh"

#include <memory>
#include <boost/asio.hpp>

namespace Nodes {
  class MasterLocal: public NodeLocal, public iPush {
  typedef boost::asio::ip::tcp::iostream _ios;
  public:
    MasterLocal();
    ~MasterLocal();

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

    private:
      virtual void main_loop() override;
  };
}
