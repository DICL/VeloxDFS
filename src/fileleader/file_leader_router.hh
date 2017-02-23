#pragma once
#include "../network/router_decorator.hh"
#include "file_leader.hh"

namespace eclipse {

//!
class FileLeaderRouter: public RouterDecorator {
  public:
    FileLeaderRouter(FileLeader*, Router*);
    ~FileLeaderRouter() = default;

  protected:
    FileLeader* file_leader;

    void insert_file(messages::Message*, Channel*);
    void update_file(messages::Message*, Channel*);
    void request_file(messages::Message*, Channel*);
    void request_ls(messages::Message*, Channel*);
    void delete_file(messages::Message*, Channel*);
    void file_exist(messages::Message*, Channel*);
    void replicate_metadata(messages::Message*, Channel*);
    void request_format(messages::Message*, Channel*);
};

} /* eclipse  */ 
