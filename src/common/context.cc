#include "context.hh"
#include <algorithm>

using namespace std;

Context& context = *Context::connect();

Context* Context::singleton = nullptr;

// Constructors {{{
Context* Context::connect (string title) {

  if (singleton == nullptr) {
    singleton = new Context(title);
    singleton->init();
    singleton->run();
  }

  return singleton;
}

Context* Context::connect () {

  if (singleton == nullptr) {
    singleton = new Context();
    singleton->init();
    singleton->run();
  }

  return singleton;
}

Context::Context(string s): settings(s), work(io) {
  init();
}

Context::Context(): work(io) {
  init();
}

Context::~Context() {
}
// }}}
// run {{{
void Context::run (){ 
  int concurrency = settings.get<int> ("cache.concurrency");
  for (int i = 0; i < concurrency; i++ ) {
    auto t = new std::thread ( [this] {
        this->io.run();
        });

    threads.emplace_back (t);
  }
}
// }}}
// join {{{
bool Context::join () {
  for (auto& t : threads) t->join();
  return true;
}
// }}}
// init {{{
void Context::init() {
  settings.load();
  string logname = settings.get<string> ("log.name");
  string logtype = settings.get<string> ("log.type");
  string ip = settings.getip();
  vector<string> nodes  = settings.get<vector<string>> ("network.nodes");
  logger.reset(Logger::connect(logname, logtype));

  id = find(nodes.begin(), nodes.end(), ip) - nodes.begin();
}
// }}}
