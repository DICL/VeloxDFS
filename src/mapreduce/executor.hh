#pragma once
#include "mr_traits.hh"
#include "peerlocalmr.hh"
#include "../messages/task.hh"

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;

class Executor: public MR_traits {
  typedef void (*maptype)(std::string);
  public:
    Executor (Context&);
    ~Executor ();

    void action (tcp::socket*) override;

  protected:
    PeerLocalMR peer_cache;
    boost::asio::streambuf inbound_data;
    char inbound_header [16];
    const int header_size = 16;

    template <typename T> void process_message (T);
    void do_read (tcp::socket* sock);
    void on_read_header (const error_code&, size_t, 
        tcp::socket*); 
    void on_read_body (const error_code&, size_t, 
        tcp::socket*); 

    void run_map (messages::Task*, std::string);
};

} /* eclipse  */ 
