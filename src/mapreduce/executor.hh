#pragma once
#include "peermr.hh"
#include "../nodes/node.hh"
#include "../messages/task.hh"

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;

class Executor: public Node, public AsyncNode {
  typedef void (*maptype)(std::string);
  public:
    Executor (Context&);
    ~Executor ();

    bool establish() override; 
    void on_connect() override;
    void on_disconnect() override;
    void on_read(messages::Message*) override;

  protected:
    PeerMR peer_cache;
    int port;

    template <typename T> void process (T);
    void run_map (messages::Task*, std::string);
//    void run_reduce (messages::Task*, std::string);
//    void run_flatmap (messages::Task*, std::string);
};

} /* eclipse  */ 
