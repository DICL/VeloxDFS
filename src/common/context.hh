#pragma once
#include "settings.hh"
#include "logger.hh"

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

extern Context& context;

#define ERROR(X, ...) context.logger->error(X, ##__VA_ARGS__ )
#define WARN(X, ...) context.logger->warn(X, ##__VA_ARGS__)
#define INFO(X, ...) context.logger->info(X, ##__VA_ARGS__)
#define DEBUG(X, ...) context.logger->debug(X, ##__VA_ARGS__)
