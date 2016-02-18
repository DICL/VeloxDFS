#pragma once
#include "network.hh"
#include "asyncnode.hh"
#include "acceptor.hh"
#include <boost/asio.hpp>
#include <map>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;

template<typename TYPE>
class AsyncNetwork: public Network {
  public:
    AsyncNetwork(AsyncNode*, Context&, size_t, int);
    ~AsyncNetwork ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*) override;
    void on_accept(int, boost::asio::ip::tcp::socket*);
    void on_disconnect() override;

  private:
    std::map<int, u_ptr<TYPE>> channels;
    Acceptor<AsyncNetwork<TYPE>> acceptor;
    int accepted_size = 0;
    AsyncNode* node;
};
// Constructor {{{
template<typename TYPE>
AsyncNetwork<TYPE>::AsyncNetwork (AsyncNode* n, Context& c,
    size_t size, int port):
  acceptor(c, port, this),
  node(n)
{
  Settings& setted = c.settings;
  vec_str nodes  = setted.get<vec_str> ("network.nodes");

  if (size == 1) {
      channels.emplace (std::make_pair(0,
            std::make_unique<TYPE>(c, 0, n )));
 
  } else {
    int i = 0;
    for (auto& node_: nodes) {
      if (i != c.id) 
        channels.emplace (i, std::make_unique<TYPE>(c, i, n));
      i++;
    }
  }
}
template<typename TYPE>
AsyncNetwork<TYPE>::~AsyncNetwork () {  }
// }}}
// establish {{{
template<typename TYPE>
bool AsyncNetwork<TYPE>::establish () {
 
  acceptor.listen();
  for (auto& channel : channels)
    channel.second->do_connect();

  return true;
}
// }}}
// close {{{
template<typename TYPE>
bool AsyncNetwork<TYPE>::close () {
  return true;
}
// }}}
// size {{{
template<typename TYPE>
size_t AsyncNetwork<TYPE>::size () {
  return channels.size();
}
// }}}
// send {{{
template<typename TYPE>
bool AsyncNetwork<TYPE>::send (int i, messages::Message* m) {
  channels[i]->do_write(m);
  return true;
}
// }}}
// on_accept {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_accept (int i,
    boost::asio::ip::tcp::socket* sock) {
  channels[i]->on_accept(sock);

  accepted_size++;

  if (accepted_size >= (int)channels.size()) node->on_connect();
}
// }}}
// on_disconnect {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_disconnect () {
  accepted_size--;
}
// }}}
}
} /* eclipse  */
