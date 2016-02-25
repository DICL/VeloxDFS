#pragma once
#include "settings.hh"
#include "logger.hh"

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <thread>

struct Context {
  boost::asio::io_service io;
  std::unique_ptr<Logger, decltype(&Logger::disconnect)>
    logger {nullptr, Logger::disconnect};
  Settings settings;
  int id;

  void run ();
  bool join ();

  Context(std::string);
  Context();
  ~Context();

  protected:
    void init();
    std::vector<std::unique_ptr<std::thread>> threads;
    boost::asio::io_service::work work;
};
