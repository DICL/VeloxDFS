// @file
// @author Vicente Adolfo Bolea Sanchez
// @brief Simple Logger interface 
//
// @usage 
//  Logger* log;
//  log = Logger::connect ("Eclipse", LOG_LOCAL7);
//
//  log->info ("Message here %d %s", 1, "sdas");
//  log->warn ("Message here %d %s", 1, "sdas");
//  log->error ("Message here %d %s", 1, "sdas");
//
//  Logger::disconnect(log);
//
#ifndef __LOGGER_HH__
#define __LOGGER_HH__

#include <string>

class Logger {
  private:
    void log (int, const char*, va_list);

    // Singleton things
    static Logger* singleton;
    Logger(std::string, std::string);
    ~Logger();

  public:
    static Logger* connect(std::string, std::string);
    static void disconnect(Logger*);

    void debug (const char* fmt, ...);
    void info (const char* fmt, ...);
    void notice (const char* fmt, ...);
    void warn (const char* fmt, ...);
    void error (const char* fmt, ...);
    void panic (const char* fmt, ...);

    void panic_if (bool, const char* fmt, ...);
    void error_if (bool, const char* fmt, ...);
};

#endif
