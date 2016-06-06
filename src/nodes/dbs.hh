#pragma once
#include "io_interface.hh"

namespace eclipse {

class DBS: public IO_interface {
  public:
    DBS(DIO*);
    ~DBS();

    bool insert (Blob*) override;
    bool open (Blob*) override;
    bool exists (Blob*) override;
    BlobInfo info (Blob*) override;
    bool rename (Blob*) override;
    bool remove (Blob*) override;
    bool format () override;
    bool list () override;

  protected:
    Directory directory;
    DIO* dio;
};

}
