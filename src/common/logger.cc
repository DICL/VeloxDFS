// @file
// @author Vicente Adolfo Bolea Sanchez
// @brief Simple Logger implementation using syslog
//
#include "logger.hh"

#include <stdarg.h>
#include <syslog.h>
#include <unordered_map>
#include <stdlib.h>
#include <string.h>
#include <boost/lexical_cast.hpp>

using std::string;

std::unordered_map<string, int> syslog_facilities {
  {"LOG_LOCAL1" , LOG_LOCAL1},
  {"LOG_LOCAL2" , LOG_LOCAL2},
  {"LOG_LOCAL3" , LOG_LOCAL3},
  {"LOG_LOCAL4" , LOG_LOCAL4},
  {"LOG_LOCAL5" , LOG_LOCAL5},
  {"LOG_LOCAL6" , LOG_LOCAL6},
  {"LOG_LOCAL7" , LOG_LOCAL7},
  {"LOG_DAEMON" , LOG_DAEMON},
  {"LOG_USER" , LOG_USER}
};

Logger* Logger::singleton = nullptr;

Logger* Logger::connect (string title, string type) {
  if (singleton == nullptr)
    singleton = new Logger(title, type);

  return singleton;
}

void Logger::disconnect (Logger* in) {
  if (singleton != nullptr) {
    delete singleton;
    singleton = nullptr;
  }
  in = nullptr;
}

Logger::Logger (string title, string type) { 
  int type_ = syslog_facilities[type];
  openlog (title.c_str() , LOG_CONS, type_); 
}

Logger::~Logger () { closelog (); }

void Logger::debug (const char* fmt, ...) { 
  va_list ap;

  va_start(ap, fmt);
  log(LOG_DEBUG, fmt, ap);
  va_end(ap);
}

void Logger::info (const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  log(LOG_INFO, fmt, ap);
  va_end(ap);
}

void Logger::notice (const char* fmt, ...) { 
  va_list ap;

  va_start(ap, fmt);
  log(LOG_NOTICE, fmt, ap);
  va_end(ap);
}

void Logger::warn (const char* fmt, ...) { 
  va_list ap;

  va_start(ap, fmt);
  log(LOG_WARNING, fmt, ap);
  va_end(ap);
}

void Logger::error (const char* fmt, ...) { 
  va_list ap;
  char msg [256];

  strncpy(msg, fmt, 256 );
  strncat(msg," [ERR:%m]", 256);
  va_start(ap, fmt);
  log(LOG_ERR, msg, ap);
  va_end(ap);
}

void Logger::panic (const char* fmt, ...) { 
  va_list ap;

  va_start(ap, fmt);
  log(LOG_EMERG, fmt, ap);
  va_end(ap);
  exit (EXIT_FAILURE);
}

void Logger::panic_if (bool cmp, const char* fmt, ...) { 
  if (cmp) {
    va_list ap;

    va_start(ap, fmt);
    log(LOG_EMERG, fmt, ap);
    va_end(ap);
    exit (EXIT_FAILURE);
  }
}

void Logger::error_if (bool cmp, const char* fmt, ...) {
  if (cmp) {
    va_list ap;
    char msg [256];

    strncpy(msg, fmt, 256 );
    strncat(msg," [ERR:%m]", 256);
    va_start(ap, fmt);
    log(LOG_ERR, msg, ap);
    va_end(ap);
  }
}

void Logger::log (int type, const char* fmt, va_list ap) { 
  vsyslog (type, fmt, ap);
}
