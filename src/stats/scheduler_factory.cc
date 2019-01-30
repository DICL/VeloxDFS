#include "logical_blocks_scheduler.hh"
#include "../common/histogram.hh"
#include "../common/context_singleton.hh"
#include "zk_listener.hh"
#include <memory>
#include <map>

using namespace std;

namespace eclipse {
namespace logical_blocks_schedulers {

shared_ptr<scheduler> scheduler_factory(string type, Histogram* boundaries, SCHEDULER_OPTS options) {
  shared_ptr<scheduler> sch;

  if (type == "scheduler_simple") {
    sch = make_shared<scheduler_simple>();

  } else if (type == "scheduler_score_based") {
    sch = make_shared<scheduler_score_based>();

  } else if (type == "python") {
    sch = make_shared<scheduler_python>();

  } else if (type == "scheduler_vlmb") {
    sch = make_shared<scheduler_vlmb>();

  } else if (type == "scheduler_multiwave") {
    sch = make_shared<scheduler_multiwave>();

  } else if (type == "scheduler_lean") {
    sch = make_shared<scheduler_lean>();

  } else {
    ERROR("No file scheduler chosen, EXITING");
  }

  sch->boundaries = boundaries;
  sch->listener = make_shared<zk_listener>();
  sch->options = options;

  return sch;
}

}
}
