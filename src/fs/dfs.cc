#include "dfs.hh"
using namespace std;
using namespace eclipse;

namespace eclipse{
  void DFS::load_settings () {
    Context con;
    BLOCK_SIZE = con.settings.get<int>("filesystem.block");
    NUM_SERVERS = con.settings.get<vector<string>>("network.nodes").size();
    path = con.settings.get<string>("path.scratch");
    replica = con.settings.get<int>("filesystem.replica");
    port = con.settings.get<int> ("network.port_mapreduce");
    nodes = con.settings.get<vector<string>>("network.nodes");
  }

  unique_ptr<tcp::socket> DFS::connect (uint32_t hash_value) { 
		auto socket = make_unique<tcp::socket>(iosvc);
    string host = nodes[ hash_value % nodes.size() ];
    tcp::resolver resolver (iosvc);
    tcp::resolver::query query (host, to_string(port));
    tcp::resolver::iterator it (resolver.resolve(query));
    auto ep = make_unique<tcp::endpoint>(*it);
    socket->connect(*ep);
    return socket;
  }

  void DFS::send_message (tcp::socket* socket, eclipse::messages::Message* msg) {
    string out = save_message(msg);
    stringstream ss;
    ss << setfill('0') << setw(16) << out.length() << out;

    socket->send(boost::asio::buffer(ss.str()));
  }

  template <typename T>
    unique_ptr<T> DFS::read_reply(tcp::socket* socket) {
      char header[17] = {0};
      header[16] = '\0';
      socket->receive(boost::asio::buffer(header, 16));
      size_t size_of_msg = atoi(header);
			vector<char> body(size_of_msg);
      socket->receive(boost::asio::buffer(body.data(), size_of_msg));
      string recv_msg(body.data(), size_of_msg);
      eclipse::messages::Message* m = load_message(recv_msg);
      return unique_ptr<T>(dynamic_cast<T*>(m));
    }

  int DFS::put(int argc, char* argv[])
  {
    Context con;
    if(argc < 3)
    {
      cout << "[INFO] dfs put file_1 file_2 ..." << endl;
      return -1;
    }
    else
    {
			vector<char> chunk(BLOCK_SIZE);
      Histogram boundaries(NUM_SERVERS, 0);
      boundaries.initialize();

      for(int i=2; i<argc; i++)
      {
        string file_name = argv[i];
        FileExist fe;
        fe.file_name = file_name;
        uint32_t file_hash_key = h(file_name);
        auto socket = connect(file_hash_key);
        send_message(socket.get(), &fe);
        auto rep = read_reply<Reply> (socket.get());

        if (rep->message == "TRUE")
        {
          cerr << "[ERR] " << file_name << " already exists." << endl;
          continue;
        }
        cout << "[INFO] " << argv[i] << " is uploaded." << endl;

        int which_server = rand()%NUM_SERVERS;
        ifstream myfile (file_name);
        uint64_t start = 0;
        uint64_t end = start + BLOCK_SIZE - 1;
        uint32_t block_size = 0;
        unsigned int block_seq = 0;

        FileInfo file_info;
        file_info.file_name = file_name;
        file_info.file_hash_key = file_hash_key;
        file_info.replica = replica;
        myfile.seekg(0, myfile.end);
        file_info.file_size = myfile.tellg();
        BlockInfo block_info;

        while(1)
        {
          if(end < file_info.file_size)
          {
            myfile.seekg(start+BLOCK_SIZE-1, myfile.beg);
            while(1)
            {
              if(myfile.peek() =='\n')
              {
                break;
              }
              else
              {
                myfile.seekg(-1, myfile.cur);
                end--;
              }
            }
          }
          block_size = (uint32_t) end - start;
          bzero(chunk.data(), BLOCK_SIZE);
          myfile.seekg(start, myfile.beg);
          block_info.content.reserve(block_size);
          myfile.read(chunk.data(), block_size);
          block_info.content = chunk.data();

          block_info.block_name = file_name + "_" + to_string(block_seq);
          block_info.file_name = file_name;
          block_info.block_seq = block_seq++;
          block_info.block_hash_key = boundaries.random_within_boundaries(which_server);
          block_info.block_size = block_size;
          block_info.is_inter = 0;
          block_info.node = "1.1.1.1";
          block_info.l_node = "1.1.1.0";
          block_info.r_node = "1.1.1.2";
          block_info.is_committed = 1;
          //block_info.node = remote_server.ip_address;
          //Node l_node = lookup((block_hash_key-1+NUM_SERVERS)%NUM_SERVERS);
          //Node r_node = lookup((block_hash_key+1+NUM_SERVERS)%NUM_SERVERS);
          //block_info.l_node = l_node.ip_address;
          //block_info.r_node = r_node.ip_address;

          send_message(socket.get(), &block_info);
          auto reply = read_reply<Reply> (socket.get());

          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          } 

          if(end >= file_info.file_size)
          {
            break;
          }
          start = end;
          end = start + BLOCK_SIZE - 1;
          which_server = (which_server + 1) % NUM_SERVERS;
        }

        file_info.num_block = block_seq;
        send_message(socket.get(), &file_info);
        auto reply = read_reply<Reply> (socket.get());

        if (reply->message != "OK") {
          cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        } 
        myfile.close();
      }
    }
    return 0;
  }

  int DFS::get(int argc, char* argv[])
  {
    if (argc < 3) {
      cout << "[INFO] dfs get file_1 file_2 ..." << endl;
      return -1;

    } else {
      Histogram boundaries(NUM_SERVERS, 0);
      boundaries.initialize();

      for (int i=2; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);
        auto socket = connect (file_hash_key % NUM_SERVERS);
        FileRequest fr;
        fr.file_name = file_name;

        send_message (socket.get(), &fr);
        auto fd = read_reply<FileDescription> (socket.get());

        ofstream f (file_name);
        int block_seq = 0;
        for (auto block_name : fd->blocks) {
          uint32_t hash_key = fd->hash_keys[block_seq++];
          auto tmp_socket = connect(boundaries.get_index(hash_key));
          BlockRequest br;
          br.block_name = block_name; 
          br.hash_key   = hash_key; 
          send_message(tmp_socket.get(), &br);
          auto msg = read_reply<BlockInfo>(tmp_socket.get());
          f << msg->content;
        }

        cout << "[INFO] " << file_name << " is downloaded." << endl;
        f.close();
      }
    }
    return 0;
  }

  int DFS::ls(int argc, char* argv[])
  {
    vector<FileInfo> total;

    for(unsigned int net_id=0; net_id<NUM_SERVERS; net_id++)
    {
      FileList file_list;
      auto socket = connect(net_id);
      send_message(socket.get(), &file_list);
      auto file_list_reply = read_reply<FileList>(socket.get());

      std::copy(file_list_reply->data.begin(), file_list_reply->data.end(), back_inserter(total));
    }

    std::sort(total.begin(), total.end(), [] (const FileInfo& a, const FileInfo& b) {
        return (a.file_name < b.file_name);
        });

    cout 
      << setw(25) << "FileName" 
      << setw(14) << "Hash Key"
      << setw(14) << "Size"
      << setw(8)  << "Blocks"
      << setw(14) << "Host"
      << setw(5)  << "Repl"
      << endl << string(80,'-') << endl;


    for (auto& fl: total) {
      cout 
        << setw(25) << fl.file_name
        << setw(14) << fl.file_hash_key
        << setw(14) << fl.file_size
        << setw(8) << fl.num_block
        << setw(14) << nodes[h(fl.file_name) % NUM_SERVERS]
        << setw(5) << fl.replica
        << endl;
    }
    return 0;
  }

  int DFS::rm(int argc, char* argv[])
  {
    if(argc < 3)
    {
      cout << "[INFO] dfs rm file_1 file_2 ..." << endl;
      return -1;
    }
    else
    {
      Histogram boundaries(NUM_SERVERS, 0);
      boundaries.initialize();

      for(int i=2; i<argc; i++)
      {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);
        auto socket = connect(file_hash_key);
        FileRequest fr;
        fr.file_name = file_name;

        send_message(socket.get(), &fr);
        auto fd = read_reply<FileDescription>(socket.get());

        unsigned int block_seq = 0;
        for (auto block_name : fd->blocks) {
          auto tmp_socket = connect(boundaries.get_index(fd->hash_keys[block_seq]));
          BlockDel bd;
          bd.block_name = block_name;
          bd.file_name = file_name;
          bd.block_seq = block_seq++;
          send_message(tmp_socket.get(), &bd);
          auto msg = read_reply<Reply>(tmp_socket.get());
          if (msg->message != "OK") {
            cerr << "[ERR] " << block_name << "doesn't exist." << endl;
            return EXIT_FAILURE;
          }

        }

        FileDel file_del;
        file_del.file_name = file_name;
        socket = connect(file_hash_key);
        send_message(socket.get(), &file_del);
        auto reply = read_reply<Reply>(socket.get());
        if (reply->message != "OK") {
          cerr << "[ERR] " << file_name << " doesn't exist." << endl;
          return EXIT_FAILURE;
        }
        cout << "[INFO] " << file_name << " is removed." << endl;
      }
      return 0;
    }
  }

  int DFS::format(int argc, char* argv[]) {
    vector<FileInfo> total;

    for (unsigned int net_id = 0; net_id < NUM_SERVERS; net_id++) {
      FormatRequest fr;
      auto socket = connect(net_id);
      send_message(socket.get(), &fr);
      auto reply = read_reply<Reply>(socket.get());

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return -1;
      } 

    }
    cout << "[INFO] dfs format is done." << endl;
    return 0;
  }
}
