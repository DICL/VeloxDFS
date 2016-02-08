#pragma once
#include "mr_traits.hh"
#include "peerlocalmr.hh"
#include "../messages/task.hh"
#include "../nodes/peerremote.hh"

namespace eclipse {

using boost::system::error_code;
using boost::asio::ip::tcp;

class Executor: public MR_traits {
  typedef void (*maptype)(std::string);
  public:
    Executor (Context&);
    ~Executor ();

    void action (tcp::socket*) override;
    void process_message (messages::Message*) override;

  protected:
    PeerLocalMR peer_cache;
    PeerRemote* peer_remote = nullptr;

    template <typename T> void process (T);
    void run_map (messages::Task*, std::string);
};

} /* eclipse  */ 
