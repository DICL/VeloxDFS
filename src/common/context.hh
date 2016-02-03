#pragma once
#include "settings.hh"
#include "logger.hh"

#include <boost/asio.hpp>
#include <string>

struct Context {
  boost::asio::io_service io;
  std::unique_ptr<Logger, decltype(&Logger::disconnect)> 
    logger {nullptr, Logger::disconnect};
  Settings settings;

  Context(std::string);
  Context();
  ~Context();

  protected:
    void init();
};
