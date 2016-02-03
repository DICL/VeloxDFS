#include "context.hh"

using namespace std;

Context::Context(string s): settings(s) {
  init();
}

Context::Context() {
  init();
}

Context::~Context() {
}

void Context::init() {
  settings.load();
  string logname = settings.get<string> ("log.name");
  string logtype = settings.get<string> ("log.type");
  logger.reset(Logger::connect(logname, logtype));
}
