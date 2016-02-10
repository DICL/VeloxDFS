#pragma once
#include "network.hh"
#include "asyncnode.hh"
#include "acceptor.hh"
#include "../messages/message.hh"
#include <boost/asio.hpp>
#include <map>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;

template<typename TYPE>
class AsyncNetwork: public Network {
  public:
    AsyncNetwork(AsyncNode*, Context&, size_t);
    ~AsyncNetwork ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*);
    bool on_accept(int, boost::asio::ip::tcp::socket*);

  private:
    std::map<int, TYPE*> channels;
    int accepted_size = 0;
    Acceptor<AsyncNetwork<TYPE>> acceptor;
    AsyncNode* node;
};
// Constructor {{{
template<typename TYPE>
AsyncNetwork<TYPE>::AsyncNetwork (AsyncNode* n, Context& c, 
    size_t size):
  acceptor(c, this),
  node(n)
{
  Settings& setted = c.settings;
  vec_str nodes  = setted.get<vec_str> ("network.nodes");

  if (size == 1) {
      channels.insert ({ 0, new TYPE{c, 0, n} });
  
  } else { 
    int i = 0;
    for (auto& node_: nodes) {
      if (i != c.id) 
        channels.insert ({i, new TYPE{c, i, n}});
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
bool AsyncNetwork<TYPE>::on_accept (int i, 
    boost::asio::ip::tcp::socket* sock) {
  channels[i]->on_accept(sock);

  accepted_size++;

  if (accepted_size >= channels.size()) node->on_connect();
}
// }}}
}
} /* eclipse  */ 
