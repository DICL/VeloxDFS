#pragma once

#include "../common/histogram.hh"
#include "../messages/filedescription.hh"
#include "stats_listener.hh"

#include <vector>
#include <string>
#include <memory>

namespace eclipse {
namespace logical_blocks_schedulers {

class scheduler;

class scheduler {
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) = 0;

    std::shared_ptr<stats_listener> listener;
    Histogram* boundaries;
};

std::shared_ptr<eclipse::logical_blocks_schedulers::scheduler> scheduler_factory(std::string, Histogram*);

class scheduler_simple: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_score_based: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

}
}
