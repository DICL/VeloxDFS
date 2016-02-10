#pragma once
#include "peerlocalmr.hh"
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
    void on_read(messages::Message*) override;

  protected:
    PeerLocalMR peer_cache;
    PeerRemote* peer_remote = nullptr;

    template <typename T> void process (T);
    void run_map (messages::Task*, std::string);
};

} /* eclipse  */ 
