#pragma once

#include "DHTdefinitions.hh"
#include "../nodes/ipull.hh"
#include "../common/histogram.hh"

#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

class DHTclient: public Nodes::iPull {
  typedef boost::asio::ip::tcp tcp;

  public:
    DHTclient (std::string&, int);
    ~DHTclient ();

    bool connect () override;
    void close () override;
    void detach () override;

    int index_of (uint64_t) const;
    int index_of (std::string) const;

  private:
    std::string master;
    std::unique_ptr<Histogram> histogram;
    bool die_thread;

    boost::asio::io_service io_service;
    std::unique_ptr<tcp::socket> socket;
    std::unique_ptr<tcp::resolver::iterator> endpoint_iterator;
    std::unique_ptr<std::thread> detached;

    void on_read_body (boost::system::error_code, size_t);
//    void on_read_header (boost::system::error_code, size_t);

    boost::asio::streambuf inbound_header_, inbound_data_;
};
