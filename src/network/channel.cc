#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (Context& c) :
  iosvc  (c.io), 
  port   (c.settings.get<int>("network.port_cache")),
  logger (c.logger.get()) 
{}

}
}
