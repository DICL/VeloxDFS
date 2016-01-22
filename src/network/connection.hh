#pragma once
#include <string>

namespace network {
template <typename T>
class Connection {
  public:
    virtual bool connect (std::string);
    virtual bool close (std::string);
    T* data() {return data_; }

    Connection(T* d) { data_ = d; }

  private:
    T* data_;
};

}
