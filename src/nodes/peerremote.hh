#pragma once

#include "noderemote.hh"
#include "../messages/message.hh"

namespace Nodes {
  class PeerRemote: public NodeRemote {
    protected:
      void on_read (); 
      void on_write (); 

    public:
      using NodeRemote::NodeRemote;
      ~PeerRemote () = default;

      void do_read (); 
      void do_write (); 

      //void insert (std::string, std::string);
      //std::string lookup (std::string);
      //bool exist (std::string);

      void send (network::Message*) override;
  };
}
