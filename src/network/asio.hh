#pragma once
#include "communication.hh"

namespace network {

class Asio: public Communication {
  public:
    bool connect (std::string, int) override;
    bool listen (int) override;
    bool close () override;
    bool send (const Message*) override;
    bool recv (Message*) override;

  public:
    Asio (boost::asio::io_service&);
    ~Asio() = default;

  private:
    boost::asio::io_service& ioservice;
};
}
