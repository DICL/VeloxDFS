#include "directory.hh"
#include "../messages/filelist.hh"
#include "../messages/boost_impl.hh"
#include "../messages/factory.hh"
#include <boost/asio.hpp>
#include <iomanip>

using namespace std;
using namespace eclipse;
using namespace eclipse::messages;
using boost::asio::ip::tcp;
using vec_str = std::vector<std::string>;

boost::asio::io_service iosvc;

tcp::socket* connect (int index) { 
  tcp::socket* socket = new tcp::socket (iosvc);
  Settings setted = Settings().load();

  int port      = setted.get<int> ("network.port_mapreduce");
  vec_str nodes = setted.get<vec_str> ("network.nodes");

  string host = nodes[ index ];

  tcp::resolver resolver (iosvc);
  tcp::resolver::query query (host, to_string(port));
  tcp::resolver::iterator it (resolver.resolve(query));
  auto ep = new tcp::endpoint (*it);
  socket->connect(*ep);
  return socket;
}

void send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
  string out = save_message(msg);
  stringstream ss;
  ss << setfill('0') << setw(16) << out.length() << out;

  socket->send(boost::asio::buffer(ss.str()));
}

eclipse::messages::FileList* read_reply(tcp::socket* socket) {
  char header[16];
  socket->receive(boost::asio::buffer(header));
  size_t size_of_msg = atoi(header);
  char* body = new char[size_of_msg];
  socket->receive(boost::asio::buffer(body, size_of_msg));
  string recv_msg(body);
  delete body;
  eclipse::messages::Message* m = load_message(recv_msg);
  return dynamic_cast<eclipse::messages::FileList*>(m);
}


int main(int argc, char* argv[])
{
  Context con;
  const int NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
  vector<string> nodes = con.settings.get<vector<string>>("network.nodes");
  vector<FileInfo> total;

  for(int net_id=0; net_id<NUM_SERVERS; net_id++)
  {
    FileList file_list;
    tcp::socket* socket = connect(net_id);
    send_message(socket, &file_list);
    auto file_list_reply = read_reply(socket);
    socket->close();

    std::copy(file_list_reply->data.begin(), file_list_reply->data.end(), back_inserter(total));
    delete file_list_reply;
  }

  std::sort(total.begin(), total.end(), [] (const FileInfo& a, const FileInfo& b) {
      return (a.file_name < b.file_name);
      });
  cout 
    << setw(14) << "FileName" 
    << setw(14) << "Hash Key"
    << setw(14) << "Size"
    << setw(14) << "NumBlocks"
    << setw(14) << "Host"
    << setw(14) << "Replicas"
    << endl << string(14*6,'-') << endl;


  for (auto& fl: total) {
    cout 
      << setw(14) << fl.file_name
      << setw(14) << fl.file_hash_key
      << setw(14) << fl.file_size
      << setw(14) << fl.num_block
      << setw(14) << nodes[h(fl.file_name) % NUM_SERVERS]
      << setw(14) << fl.replica
      << endl;
  }
  return 0;
}
