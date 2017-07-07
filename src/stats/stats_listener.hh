#pragma once
#include <vector>

namespace eclipse {

class stats_listener {
  public:
    virtual ~stats_listener () = default;

    virtual std::vector<double> get_io_stats() = 0;
};
  
} /*  eclipse  */ 
