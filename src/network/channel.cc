#include "channel.hh"

using namespace eclipse::network;

Channel::Channel () :
  iosvc  (context.io), 
  port   (context.settings.get<int>("network.ports.internal")),
  logger (context.logger.get()) 
{}
