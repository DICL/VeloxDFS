#pragma once

#include "../common/histogram.hh"
#include "../messages/filedescription.hh"
#include "stats_listener.hh"

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace eclipse {
namespace logical_blocks_schedulers {
typedef std::map<std::string, std::string> SCHEDULER_OPTS;

class scheduler;

class scheduler {
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) = 0;

    std::shared_ptr<stats_listener> listener;
    std::map<std::string, std::string> options;
    Histogram* boundaries;
};

std::shared_ptr<eclipse::logical_blocks_schedulers::scheduler>
    scheduler_factory(std::string, Histogram*, SCHEDULER_OPTS = SCHEDULER_OPTS());

class scheduler_simple: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_score_based: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_python: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_vlmb: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_multiwave: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

class scheduler_lean: public scheduler{
  public:
    virtual void generate(messages::FileDescription& file_desc, std::vector<std::string> nodes) override;
};

}
}
