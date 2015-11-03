#pragma once

#include "nodelocal.hh"
#include "ipush.hh"
#include "../cache/cache.hh"
#include "../common/histogram.hh"

#include <string>
#include <thread>
#include <boost/asio.hpp>

using std::string;

namespace Nodes {
class PeerLocal: public NodeLocal {
  protected:
    std::unique_ptr<cache::lru_cache<string, string> > cache;
    std::unique_ptr<std::thread> detached;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::unique_ptr<Histogram> histogram;
    std::map<int, boost::asio::ip::tcp::socket*> sockets_list;
    int port;

    void do_accept(); 
    void on_accept(boost::asio::ip::tcp::socket*,
        const boost::system::error_code&); 
    //void on_read (boost::system::error_code, size_t);

  public:
    PeerLocal();
    ~PeerLocal();

    void listen ();
    void accept ();

    void insert (std::string, std::string);
    std::string lookup (std::string);
    bool exist (std::string);
    void close ();

    template <typename T> void process_message (T);

};
}
