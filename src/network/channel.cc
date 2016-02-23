#include "channel.hh"

namespace eclipse {
namespace network {

Channel::Channel (Context& c, int id_) :
  iosvc  (c.io), 
  id     (id_),
  port   (c.settings.get<int>("network.port_cache")),
  logger (c.logger.get()) 
{}

}
}
