#include "context.hh"
#include <algorithm>
#include <boost/exception/exception.hpp>
#include <exception>
#include <boost/exception/all.hpp>


using namespace std;

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
          try {
            this->io.run();

          } catch (exception& e) {
            logger->error("iosvc exception %s", e.what());
          } catch (boost::exception& e) {
            logger->error("iosvc exception %s", diagnostic_information(e).c_str());
          }
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
  string logmask = settings.get<string> ("log.mask");
  string ip = settings.getip();
  vector<string> nodes  = settings.get<vector<string>> ("network.nodes");
  logger.reset(Logger::connect(logname, logtype, logmask));

  id = find(nodes.begin(), nodes.end(), ip) - nodes.begin();
  histogram = make_unique<Histogram>(nodes.size(), 100);
  histogram->initialize();
}
// }}}
