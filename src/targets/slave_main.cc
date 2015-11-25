#include "../nodes/peerlocal.hh"

int main() {
  Nodes::network net = Nodes::network::asio;
  Nodes::PeerLocal p (net);
  return 0;
}
