#include "dfs.hh"
using namespace std;
using namespace eclipse;

namespace eclipse{
  void DFS::load_settings () {
    BLOCK_SIZE = context.settings.get<int>("filesystem.block");
    NUM_SERVERS = context.settings.get<vector<string>>("network.nodes").size();
    path = context.settings.get<string>("path.scratch");
    replica = context.settings.get<int>("filesystem.replica");
    port = context.settings.get<int> ("network.port_mapreduce");
    nodes = context.settings.get<vector<string>>("network.nodes");
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
    auto DFS::read_reply(tcp::socket* socket) {
      using namespace boost::asio;
      char header[17] = {0};
      header[16] = '\0';

      read(*socket, buffer(header, 16));
      size_t size_of_msg = atoi(header);

      vector<char> body(size_of_msg);
      read(*socket, buffer(body.data(), size_of_msg));

      string recv_msg(body.data());
      T* m = dynamic_cast<T*>(load_message(recv_msg));

      return unique_ptr<T>(m);
    }

  int DFS::put(int argc, char* argv[])
  {
    if(argc < 3)
    {
      cout << "[INFO] dfs put file_1 file_2 ..." << endl;
      return -1;
    }
    else
    {
      srand((uint32_t)time(NULL));
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
          block_info.node = nodes[which_server];
          block_info.l_node = nodes[(which_server-1+NUM_SERVERS)%NUM_SERVERS];
          block_info.r_node = nodes[(which_server+1+NUM_SERVERS)%NUM_SERVERS];
          block_info.is_committed = 1;

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
        myfile.close();
        socket->close();

        if (reply->message != "OK") {
          cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        } 
        cout << "[INFO] " << argv[i] << " is uploaded." << endl;
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
        socket->close();
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
          tmp_socket->close();
        }

        cout << "[INFO] " << file_name << " is downloaded." << endl;
        f.close();
        socket->close();
      }
    }
    return 0;
  }

  int DFS::ls(int argc, char* argv[])
  {
    vector<FileInfo> total; 
    string op = "";
    if(argc >= 3)
    {
      op = argv[2];
    }
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
        << setw(14) << fl.file_hash_key;
      if(op.compare("-h") == 0) {
        uint32_t KB = 1024;
        uint32_t MB = 1024 * 1024;
        uint32_t GB = 1024 * 1024 * 1024;
        uint64_t TB = (uint64_t) 1024 * 1024 * 1024 * 1024;
        uint64_t PB = (uint64_t) 1024 * 1024 * 1024 * 1024 * 1024;
        float hsize = 0;
        int tabsize = 12;
        string unit;
        cout.precision(1);
        if(fl.file_size <= KB)
        {
          hsize = (float)fl.file_size;
          unit = "B";
          tabsize++;
          cout.precision(0);
        }
        else if(fl.file_size <= MB)
        {
          hsize = (float)fl.file_size / KB;
          unit = "KB";
        }
        else if(fl.file_size <= GB)
        {
          hsize = (float)fl.file_size / MB;
          unit = "MB";
        }
        else if(fl.file_size <= TB)
        {
          hsize = (float)fl.file_size / GB;
          unit = "GB";
        }
        else if(fl.file_size <= PB)
        {
          hsize = (float)fl.file_size / TB;
          unit = "TB";
        }
        else
        {
          hsize = (float)fl.file_size / PB;
          unit = "PB";
        }
        cout << fixed;
        cout << setw(tabsize) << hsize << unit;
      }
      else {
        cout << setw(14) << fl.file_size;
      }

      cout
        << setw(8) << fl.num_block
        << setw(14) << nodes[fl.file_hash_key % NUM_SERVERS]
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
        socket->close();

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
          socket->close();
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

  int DFS::show(int argc, char* argv[]) {
    if (argc < 3) {
      cout << "usage: dfs bl file_name1 file_name2 ..." << endl;
      return EXIT_FAILURE;
    } else {
      Histogram boundaries(NUM_SERVERS, 0);
      boundaries.initialize();
      for (int i=2; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);
        auto socket = connect (file_hash_key);
        FileRequest fr;
        fr.file_name = file_name;

        send_message (socket.get(), &fr);
        auto fd = read_reply<FileDescription>(socket.get());
        cout << file_name << endl;
        int block_seq = 0;
        for (auto block_name : fd->blocks) {
          uint32_t hash_key = fd->hash_keys[block_seq++];
          string node = nodes[boundaries.get_index(hash_key)];
          cout << "\t- " << setw(15) << block_name << " : " << setw(15) << node << endl;
        }
        socket->close(); 
      }
    }
    return EXIT_SUCCESS;
  }
}
