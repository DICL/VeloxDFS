#pragma once
#include "network.hh"
#include "asyncnode.hh"
#include "netobserver.hh"
#include "acceptor.hh"
#include "connector.hh"
#include <boost/asio.hpp>
#include <map>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;
using tcp::socket  = boost::asio::ip::tcp::socket;

template<typename TYPE>
class AsyncNetwork: public Network, public NetObserver {
  public:
    AsyncNetwork(AsyncNode*, Context&, int);
    ~AsyncNetwork ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*) override;
    void on_accept(tcp::socket*) override;
    void on_connect(tcp::socket*) override;
    void on_disconnect() override;

  private:
    int id_of(tcp::socket*);
    void start_reading();
    int completed_network();

    Context& context;
    AsyncNode* node;
    vec_str nodes;

    Acceptor acceptor;
    Connector connector;
    int accepted_size = 0, connected_size = 0, net_size = 0;

    std::map<int, std::pair<tcp::socket*, tcp::socket*> sockets;
    std::map<int, u_ptr<TYPE>> channels;
};
// Constructor {{{
template<typename TYPE>
AsyncNetwork<TYPE>::AsyncNetwork (AsyncNode* n, Context& c, int port):
  context(c),
  node(n),
  nodes(c.settings.get<vec_str> ("network.nodes")),
  acceptor(c, port, this),
  connector(c, port, this)
{ 
 if (TYPE::is_multiple())
   net_size = nodes.size();
 
 else 
   net_size = 1;
}
template<typename TYPE>
AsyncNetwork<TYPE>::~AsyncNetwork () {  }
// }}}
// establish {{{
template<typename TYPE>
bool AsyncNetwork<TYPE>::establish () {
  acceptor.listen();

  if (TYPE::is_multiple()) connector.establish();

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
void AsyncNetwork<TYPE>::on_accept (tcp::socket* sock) {
  auto i = id_of (sock);

  if (sockets.find(i) != sockets.end())
    sockets.insert(i, {nullptr, sock})
  else
    sockets[i].second = sock; 
    
  accepted_size++;

  if (is_completed_network) start_reading();
}
// }}}
// on_connect {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_connect (tcp::socket* sock) {
  auto i = id_of (sock);

  if (sockets.find(i) != sockets.end())
    sockets.insert(i, {sock, nullptr})
  else
    sockets[i].first= sock; 
    
  connected_size++;

  if (is_completed_network) start_reading();
}
// }}}
// on_disconnect {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_disconnect (tcp::socket* sock) {
  if (TYPE::is_multiple())
    connected_size--;

  accepted_size--;
}
// }}}
// completed_network {{{
bool AsyncNetwork::is_completed_network () {
  if (TYPE::is_multiple() and accepted_size >= net_size  and connected_size >= net_size)
    return true;

  if (not TYPE::is_multiple() and accepted_size >= 1) return true;

  return false;
}
// }}}
// id_of {{{
int AsyncNetwork::id_of (tcp::socket* sock) {
  auto ip = sock->remote_endpoint().address().to_string();

  auto idx = std::find(nodes.begin(), nodes.end(), ip) - nodes.begin();
  return idx;
}
// }}}
// start_reading {{{
void AsyncNetwork::start_reading () {
    for (auto sp : sockets) {
      if (TYPE::is_multiple())
        channels.insert (sp.first, {sp.second.first, sp.second.second, context, node});
      else 
        channels.insert (sp.first, {sp.second.second, context, node});
    }
    sockets.clear();
    for (auto channel : channels)
      channel->do_read();
}
// }}}
}
} /* eclipse  */
