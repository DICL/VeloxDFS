#include "channel.hh"

using namespace eclipse::network;

Channel::Channel () :
  iosvc  (context.io), 
  port   (context.settings.get<int>("network.port_cache")),
  logger (context.logger.get()) 
{}
