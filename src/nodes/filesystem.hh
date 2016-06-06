#pragma once
#include "io_interface.hh"
#include "dio.hh"

namespace eclipse {

class DFS: public IO_interface {
  public:
    DFS (DIO*);
    ~DFS();

    bool insert (Blob*) override;
    Blob* open (Path*) override;
    bool exists (Path*) override;
    Blob info (Path*) override;
    bool rename (Path*, Path*) override;
    bool remove (Path*) override;
    bool format () override;
    bool list () override;

  protected:
    Directory directory;
    DIO* dio;
    //DBS* dbs;
};

}
