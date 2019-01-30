#pragma once
#include "settings.hh"
#include "logger.hh"
#include "histogram.hh"

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <thread>

class Context {
  public:
    boost::asio::io_service io;
    std::unique_ptr<Logger, decltype(&Logger::disconnect)>
      logger {nullptr, Logger::disconnect};
    Settings settings;
    std::unique_ptr<Histogram> histogram;
    int id;

    void run ();
    bool join ();

    static Context* singleton;
    static Context* connect(std::string);
    static Context* connect();

  protected:
    Context(std::string);
    Context();
    ~Context();

    void init();
    std::vector<std::unique_ptr<std::thread>> threads;
    boost::asio::io_service::work work;
};
