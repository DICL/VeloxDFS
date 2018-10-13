#pragma once
#include <vector>
#include <utility>

namespace eclipse {

class stats_listener {
  public:
    virtual ~stats_listener () = default;

    virtual std::vector<std::pair<double,int>> get_io_stats() = 0;
};
  
} /*  eclipse  */ 
