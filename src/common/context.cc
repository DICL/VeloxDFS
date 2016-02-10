#include "context.hh"
#include <algorithm>

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
  string ip = settings.getip();
  vector<string> nodes  = settings.get<vector<string>> ("network.nodes");
  logger.reset(Logger::connect(logname, logtype));

  id = find(nodes.begin(), nodes.end(), ip) - nodes.begin();
}
