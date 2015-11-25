#include <boost/mpi.hpp>
#include <iostream>

int main(int argc, char* argv[]) 
{
  boost::mpi::environment env(argc, argv);
  network::communication*  mpi_comm = new  network::mpi(env); // net = Nodes::network::mpi(env);
  Nodes::PeerLocal p (mpi_comm);
  return 0;
}
