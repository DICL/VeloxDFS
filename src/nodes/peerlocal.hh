#pragma once

#include "nodelocal.hh"
#include "ipush.hh"
#include "../cache/cache.hh"
#include "../common/histogram.hh"

#include <string>
#include <thread>
#include <boost/asio.hpp>

namespace eclipse {

using std::string;
using std::thread;
using std::map;
using boost::asio::ip::tcp;

class PeerLocal: public NodeLocal {
  protected:
    u_ptr<lru_cache<string, string> > cache { new lru_cache<string, string> (CACHESIZE) };
    u_ptr<thread> detached;
    u_ptr<tcp::acceptor> acceptor;
    u_ptr<Histogram> histogram;

    map<int, tcp::socket*> sockets_list;
    int port;

    void do_accept (); 
    void on_accept (tcp::socket*, const boost::system::error_code&); 

  public:
    PeerLocal ();
    ~PeerLocal ();

    void listen ();
    void accept ();

    void insert (string, string);
    string lookup (string);
    bool exist (string);
    void close ();

    template <typename T> void process_message (T);
};
}
