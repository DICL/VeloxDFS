#pragma once
#include "network.hh"
#include "asyncnode.hh"
#include "netobserver.hh"
#include "acceptor.hh"
#include "connector.hh"
#include <boost/asio.hpp>
#include <mutex>
#include <map>

namespace eclipse {
namespace network {

using vec_str = std::vector<std::string>;
using boost::asio::ip::tcp;

template<typename TYPE>
class AsyncNetwork: public Network, public NetObserver {
  public:
    AsyncNetwork(AsyncNode*, int);
    ~AsyncNetwork ();

    bool establish() override;
    bool close () override;
    size_t size () override;
    bool send(int, messages::Message*) override;
    void on_accept(tcp::socket*) override;
    void on_connect(tcp::socket*) override;
    void on_disconnect(tcp::socket*, int) override;
    void on_read(messages::Message*, int) override;

  private:
    int id_of(tcp::socket*);
    void start_reading();
    bool is_completed_network();

    AsyncNode* node;
    vec_str nodes;

    Acceptor acceptor;
    Connector connector;
    int connected_size = 0, net_size = 0;

    std::map<int, std::pair<tcp::socket*, tcp::socket*>> sockets;
    std::map<int, u_ptr<TYPE>> channels;
    std::atomic<int> accepted_size;
    std::mutex acceptor_mutex;
};
// Constructor {{{
template<typename TYPE>
AsyncNetwork<TYPE>::AsyncNetwork (AsyncNode* n, int port):
  node(n),
  nodes(context.settings.get<vec_str> ("network.nodes")),
  acceptor(port, this),
  connector(port, this),
  accepted_size(0)
{ 
 if (TYPE::is_multiple())
   net_size = nodes.size() - 1;
 
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
  acceptor_mutex.lock(); 
  channels[i]->do_write(m);
  acceptor_mutex.unlock(); 
  return true;
}
// }}}
// on_accept {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_accept (tcp::socket* sock) {
  if (not TYPE::is_multiple()) {
    acceptor_mutex.lock(); 
    channels.emplace (accepted_size.load(), std::make_unique<TYPE> (sock, sock, this, accepted_size.load()));
    accepted_size++;
    channels[accepted_size.load() - 1]->do_read();
    acceptor_mutex.unlock(); 

  } else {
    auto i = id_of (sock);
    if (sockets.find(i) == sockets.end())
      sockets.insert({i, {nullptr, sock}});
    else
      sockets[i].second = sock; 
    
    accepted_size++;

    if (is_completed_network()) 
      start_reading();
  }
}
// }}}
// on_connect {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_connect (tcp::socket* sock) {
  auto i = id_of (sock);

  if (sockets.find(i) == sockets.end())
    sockets.insert({i, {sock, nullptr}});
  else
    sockets[i].first= sock; 
    
  connected_size++;

  if (is_completed_network()) start_reading();
}
// }}}
// on_disconnect {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::on_disconnect (tcp::socket* sock, int id) {
  if (TYPE::is_multiple())
    connected_size--;

  accepted_size--;

  acceptor_mutex.lock(); 
  channels.erase(id);
  acceptor_mutex.unlock(); 
}
// }}}
// completed_network {{{
template<typename TYPE>
bool AsyncNetwork<TYPE>::is_completed_network () {
  if (TYPE::is_multiple() and accepted_size.load() >= net_size  and connected_size >= net_size)
    return true;

  if (not TYPE::is_multiple() and accepted_size.load() >= 1) return true;

  return false;
}
// }}}
// id_of {{{
template<typename TYPE>
int AsyncNetwork<TYPE>::id_of (tcp::socket* sock) {
  auto ip = sock->remote_endpoint().address().to_string();

  auto idx = std::find(nodes.begin(), nodes.end(), ip) - nodes.begin();
  return idx;
}
// }}}
// start_reading {{{
template<typename TYPE>
void AsyncNetwork<TYPE>::start_reading () {
  acceptor_mutex.lock(); 

  if (TYPE::is_multiple()) {

    for (auto& sp : sockets) {
      if (channels.find(sp.first) != channels.end())
        channels.erase(sp.first);

      channels.emplace (sp.first, std::make_unique<TYPE> (sp.second.first, sp.second.second, this, sp.first));
    }
    sockets.clear();
    for (auto& channel : channels)
      channel.second->do_read();
  }

  if (not TYPE::is_multiple())
    channels[accepted_size.load() - 1]->do_read();

  acceptor_mutex.unlock(); 
  node->on_connect();
}
// }}}
// on_read {{{
template <typename TYPE>
void AsyncNetwork<TYPE>::on_read (messages::Message* m , int id) {
  node->on_read(m, id);
}
// }}}
}
} /* eclipse  */
