#include "logical_blocks_scheduler.hh"
#include "../common/histogram.hh"
#include "../common/context_singleton.hh"
#include "zk_listener.hh"
#include <memory>

namespace eclipse {
namespace logical_blocks_schedulers {

using namespace std;
std::shared_ptr<scheduler> scheduler_factory(string type, Histogram* boundaries) {
  shared_ptr<scheduler> sch;

  if (type == "scheduler_simple") {
    sch = make_shared<scheduler_simple>();

  } else if (type == "scheduler_score_based") {
    sch = make_shared<scheduler_score_based>();

  } else {
    PANIC("No file scheduler chosen, EXITING");
  }

  sch->boundaries = boundaries;
  sch->listener = make_shared<zk_listener>();

  return sch;
}

}
}
