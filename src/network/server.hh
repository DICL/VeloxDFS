#pragma once

namespace eclipse {
namespace network {

class Server: public Channel {
  public:
    using Channel::Channel;
    ~Server () = default;

    bool establish () override;
    void close () override;

  private:
    /* data */
};

}
}
