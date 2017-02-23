#pragma once

#include "../messages/message.hh"
#include "channel.hh"

namespace eclipse {
namespace network {

struct NetObserver {
  virtual ~NetObserver() { };
  virtual void on_read (messages::Message*, Channel*) = 0;
};

}
}
