#include "../common/context.hh"
#include "dfs.hh"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <ext/stdio_filebuf.h>
#include <fcntl.h>

using namespace std;
using namespace eclipse;
using namespace boost::archive;

namespace eclipse {
  // DFS {{{
  DFS::DFS() : iosvc(context.io) {}
  // }}}
  // load_settings {{{
  void DFS::load_settings() {
    BLOCK_SIZE = context.settings.get<int>("filesystem.block");
    nodes = context.settings.get<vector<string>>("network.nodes");
    replica = context.settings.get<int>("filesystem.replica");
    port = context.settings.get<int>("network.ports.client");
    NUM_NODES = nodes.size();
  }
  // }}}
  // connect {{{
  unique_ptr<tcp::socket> DFS::connect(uint32_t hash_key) { 
    auto socket = make_unique<tcp::socket>(iosvc);
    string host = nodes[hash_key % NUM_NODES];
    tcp::resolver resolver(iosvc);
    tcp::resolver::query query(host, to_string(port));
    tcp::resolver::iterator it(resolver.resolve(query));
    auto ep = make_unique<tcp::endpoint>(*it);
    socket->connect(*ep);
    return socket;
  }
  // }}}
  // send_message {{{
  void DFS::send_message(tcp::socket* socket, eclipse::messages::Message* msg) {
    string* to_send = save_message(msg);
    socket->send(boost::asio::buffer(*to_send));
  }
  // }}}
  // read_reply {{{
  template <typename T>
    auto DFS::read_reply(tcp::socket* socket) {
      using namespace boost::asio;
      char header[header_size + 1] = {0};
      header[header_size] = '\0';
      boost::asio::streambuf buf;
      read(*socket, buffer(header, header_size));
      size_t size_of_msg = atoi(header);
      read(*socket, buf, transfer_exactly(size_of_msg));
      Message* msg = nullptr;
      msg = load_message(buf);
      T* m = dynamic_cast<T*>(msg);
      return unique_ptr<T>(m);
    }
  // }}}
  // check_local_exist {{{
  bool DFS::check_local_exist(string name) {
    ifstream f(name.c_str());
    return f.good();
  }
  // }}}
  // check_exist {{{
  bool DFS::check_exist(string name, string type, uint32_t hash_key) {
    auto socket = connect(hash_key);
    CheckExist ce(name, type);
    send_message(socket.get(), &ce);
    auto rep = read_reply<Reply>(socket.get());
    socket->close();
    return (rep->message == "TRUE");
  }
  // }}}
  // put {{{
  int DFS::put(int argc, char** argv) {
    if (argc < 3) {
      cout << "[INFO] dfs put file_1 file_2 ..." << endl;
      return EXIT_FAILURE;
    } else {
      Histogram boundaries(NUM_NODES, 0);
      boundaries.initialize();
      string op = argv[2];
      int i=2;
      if (op.compare("-b") == 0) {
        replica = NUM_NODES;
        i++;
      }
      for (; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);

        // check local exist
        if (check_local_exist(file_name) == false) {
          cerr << "[ERR] " << file_name << " doesn't exist in local file system." << endl;
          continue;
        }
        
        // check exist
        auto socket = connect(file_hash_key);
        CheckExist ce(file_name, "file");
        send_message(socket.get(), &ce);
        auto rep = read_reply<Reply>(socket.get());
        if (rep->message == "TRUE") {
          cerr << "[ERR] " << file_name << " already exists." << endl;
          socket->close();
          continue;
        }
        
        // split file to blocks
        int which_node = file_hash_key % NUM_NODES;
        int ori_node = which_node;
        int fd = open(file_name.c_str(), 0);
        __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in | std::ios::binary); // 1
        istream myfile(&filebuf);
        uint64_t start = 0;
        uint64_t end = start + BLOCK_SIZE - 1;
        uint64_t block_size = 0;
        uint64_t seq = 0;
        myfile.seekg(0, myfile.end);
        uint64_t file_size = myfile.tellg();

        while (1) {
          if (end < file_size) {
            myfile.seekg(start+BLOCK_SIZE-1, myfile.beg);
            while (1) {
              if (myfile.peek() =='\n') {
                break;
              } else {
                myfile.seekg(-1, myfile.cur);
                end--;
              }
            }
          } else {
            end = file_size;
          }
          block_size = (uint64_t) end - start;

          myfile.seekg(start, myfile.beg);
          string str(block_size, '\0');
          str.reserve(block_size);
          myfile.read(&str[0], block_size);
          string name = file_name + "_" + to_string(seq);
          uint32_t hash_key = boundaries.random_within_boundaries(which_node);
          BlockInfo bi(name, hash_key, file_name, seq++, block_size, ori_node, replica, 1, str);
          posix_fadvise(fd, end, block_size, POSIX_FADV_WILLNEED);

          // send block info to leader node
          // TODO need to reduce time in send and receive part.
          send_message(socket.get(), &bi);
          auto reply = read_reply<Reply> (socket.get());
          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          }

          // prepare next loop
          if (end >= file_size) {
            break;
          }
          start = end;
          end = start + BLOCK_SIZE - 1;
          which_node = (which_node + 1) % NUM_NODES;
        }

        // send file info to leader node
        close(fd);
        FileInfo file_info(file_name, file_hash_key, file_size, seq, "block", replica);
        send_message(socket.get(), &file_info);
        auto reply = read_reply<Reply> (socket.get());
        socket->close();
        if (reply->message != "OK") {
          cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        } 
        cout << "[INFO] " << argv[i] << " is uploaded." << endl;
      }
    }
    return EXIT_SUCCESS;
  }
  // }}}
  // get {{{
  int DFS::get(int argc, char** argv) {
    if (argc < 3) {
      cout << "[INFO] dfs get file_1 file_2 ..." << endl;
      return EXIT_FAILURE;
    } else {
      for (int i=2; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);
        // check local exist
        if (check_local_exist(file_name) == true) {
          cerr << "[ERR] " << file_name << " exists in local file system." << endl;
          continue;
        }
        // check existence
        auto socket = connect(file_hash_key);
        CheckExist ce(file_name, "file");
        send_message(socket.get(), &ce);
        auto rep = read_reply<Reply>(socket.get());
        if (rep->message != "TRUE") {
          socket->close();
          cerr << "[ERR] " << file_name << " doesn't exist." << endl;
          continue;
        }

        // receive file description
        FileRequest fr(file_name);
        send_message(socket.get(), &fr);
        auto fd = read_reply<FileDescription> (socket.get());

        // collect blocks and write them down to one file
        ofstream f(file_name);
        for (vector<int>::size_type j=0; j<fd->net_id.size(); j++) {
          socket = connect(fd->net_id[j]);
          BlockRequest br(fd->block_name[j], fd->net_id[j]);
          send_message(socket.get(), &br);
          auto msg = read_reply<BlockInfo>(socket.get());
          f << msg->content;
        }
        socket->close();
        f.close();
        cout << "[INFO] " << file_name << " is downloaded." << endl;
      }
    }
    return EXIT_SUCCESS;
  }
  // }}}
  // cat {{{
  int DFS::cat(int argc, char* argv[]) {
    if (argc < 3) {
      cout << "[INFO] dfs cat file_1 file_2 ..." << endl;
      return EXIT_FAILURE;
    } else {
      for (int i=2; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);
        // check local exist
        if (check_local_exist(file_name) == true) {
          cerr << "[ERR] " << file_name << " exists in local file system." << endl;
          continue;
        }
        // check existence
        auto socket = connect(file_hash_key);
        CheckExist ce(file_name, "file");
        send_message(socket.get(), &ce);
        auto rep = read_reply<Reply>(socket.get());
        if (rep->message != "TRUE") {
          socket->close();
          cerr << "[ERR] " << file_name << " doesn't exist." << endl;
          continue;
        }

        // receive file description
        FileRequest fr(file_name);
        send_message(socket.get(), &fr);
        auto fd = read_reply<FileDescription> (socket.get());

        // collect blocks and display them
        for (vector<int>::size_type j=0; j<fd->net_id.size(); j++) {
          socket = connect(fd->net_id[j]);
          BlockRequest br(fd->block_name[j], fd->net_id[j]);
          send_message(socket.get(), &br);
          auto msg = read_reply<BlockInfo>(socket.get());
          cout << msg->content;
          socket->close();
        }
        cout << "[INFO] " << file_name << " is downloaded." << endl;
      }
    }
    return EXIT_SUCCESS;
  }
  // }}}
  // ls {{{
  int DFS::ls(int argc, char* argv[]) {
    vector<FileInfo> total; 
    string op = "";
    if (argc >= 3) {
      op = argv[2];
    }

    // collect file info from every node
    for (int net_id=0; net_id<NUM_NODES; net_id++) {
      FileList file_list;
      auto socket = connect(net_id);
      send_message(socket.get(), &file_list);
      auto file_list_reply = read_reply<FileList>(socket.get());
      total.insert(total.end(), file_list_reply->data.begin(), file_list_reply->data.end());
    }

    // sort the file list
    std::sort(total.begin(), total.end(), [] (const FileInfo& a, const FileInfo& b) {
        return (a.name < b.name);
        });

    // display the list
    cout 
      << setw(25) << "FileName" 
      << setw(14) << "Hash Key"
      << setw(14) << "Size"
      << setw(8)  << "Chunks"
      << setw(14) << "Type"
      << setw(5)  << "Repl"
      << endl << string(80,'-') << endl;

    for (auto& fl: total) {
      cout 
        << setw(25) << fl.name
        << setw(14) << fl.hash_key;
      if (op.compare("-h") == 0) {
        float hsize = 0;
        int tabsize = 12;
        string unit;
        cout.precision(2);
        if (fl.size < K) {
          hsize = (float)fl.size;
          unit = "B";
          tabsize++;
          cout.precision(0);
        } else if (fl.size < M) {
          hsize = (float)fl.size / KB;
          unit = "KB";
        } else if (fl.size < G) {
          hsize = (float)fl.size / MB;
          unit = "MB";
        } else if (fl.size < T) {
          hsize = (float)fl.size / GB;
          unit = "GB";
        } else if (fl.size < P) {
          hsize = (float)fl.size / TB;
          unit = "TB";
        } else {
          hsize = (float)fl.size / PB;
          unit = "PB";
        }
        cout << fixed;
        cout << setw(tabsize) << hsize << unit;
      } else {
        cout << setw(14) << fl.size;
      }
      cout
        << setw(8) << fl.num_block
        << setw(14) << fl.type
        << setw(5) << fl.replica
        << endl;
    }
    return EXIT_SUCCESS;
  }
  // }}}
  // rm {{{
  int DFS::rm(int argc, char* argv[]) {
    if (argc < 3) {
      cout << "[INFO] dfs rm file_1 file_2 ..." << endl;
      return EXIT_FAILURE;
    } else {
      for (int i=2; i<argc; i++) {
        string file_name = argv[i];
        uint32_t file_hash_key = h(file_name);

        // check existence
        auto socket = connect(file_hash_key);
        CheckExist ce(file_name, "file");
        send_message(socket.get(), &ce);
        auto rep = read_reply<Reply>(socket.get());
        if (rep->message != "TRUE") {
          socket->close();
          cerr << "[ERR] " << file_name << " doesn't exist." << endl;
          continue;
        }

        // receive node description
        NodeRequest nr(file_name);
        send_message(socket.get(), &nr);
        auto nd = read_reply<NodeDescription> (socket.get());
        // remove file, block, and node metadata
        FileDel fdel(file_name);
        send_message(socket.get(), &fdel);
        auto reply = read_reply<Reply> (socket.get());
        if (reply->message != "OK") {
          cerr << "[ERR] Failed to remove file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        }

        // remove blocks
        bool change_socket = true;
        for (vector<NodeInfo>::size_type j=0; j<nd->node_list.size(); j++) {
          if (change_socket == true) {
            socket = connect(nd->node_list[j].net_id);
            change_socket = false;
          }
          BlockDel bd(nd->node_list[j].name);
          send_message(socket.get(), &bd);
          auto reply = read_reply<Reply> (socket.get());
          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          }
          if (j<nd->node_list.size()-1) {
            if (nd->node_list[j].net_id != nd->node_list[j+1].net_id) {
              change_socket = true;
            }
          }
        }
        socket->close();
        cout << "[INFO] " << file_name << " is removed." << endl;
      }
      return EXIT_SUCCESS;
    }
  }
  // }}}
  // format {{{
  int DFS::format(int argc, char* argv[]) {
    FormatRequest fr;
    for (int net_id=0; net_id<NUM_NODES-1; net_id++) {
      auto socket = connect(net_id);
      send_message(socket.get(), &fr);
      socket->close();
    }
    auto socket = connect(NUM_NODES-1);
    send_message(socket.get(), &fr);
    auto reply = read_reply<Reply>(socket.get());
    socket->close();
    if (reply->message != "OK") {
      cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
      return EXIT_FAILURE;
    } 
    cout << "[INFO] dfs format is done." << endl;
    return EXIT_SUCCESS;
  }
  // }}}
// update {{{
int DFS::update(int argc, char* argv[]) {
  string ori_file_name = "";
  if (argc < 5) {
    cout << "[INFO] dfs update original_file new_file start_offset" << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    ori_file_name = argv[2];
    string new_file_name = argv[3];

    // check local exist
    if (check_local_exist(new_file_name) == false) {
      cerr << "[ERR] " << new_file_name << " doesn't exist in local file system." << endl;
      return EXIT_FAILURE;
    }
    uint64_t start_offset = (uint64_t)atoi(argv[4]);

    // check existence
    uint32_t file_hash_key = h(ori_file_name);
    auto socket = connect(file_hash_key);
    CheckExist ce(ori_file_name, "file");
    send_message(socket.get(), &ce);
    auto rep = read_reply<Reply>(socket.get());
    if (rep->message != "TRUE") {
      cerr << "[ERR] " << ori_file_name << " doesn't exist." << endl;
      socket->close();
      return EXIT_FAILURE;
    }

    // receive file description
    FileRequest fr(ori_file_name);
    ifstream myfile(new_file_name);
    myfile.seekg(0, myfile.end);
    uint64_t new_file_size = myfile.tellg();
    send_message(socket.get(), &fr);
    auto fd = read_reply<FileDescription> (socket.get());
    if (start_offset + new_file_size > fd->size) {
      cerr << "[ERR] Wrong file size." << endl;
      return EXIT_FAILURE;
    }

    // main process
    myfile.seekg(0, myfile.beg);
    char *buffer = new char[new_file_size];
    myfile.read(buffer, new_file_size);
    string sbuffer(buffer);
    delete[] buffer;
    myfile.close();

    uint64_t seq = 0;
    uint64_t passed_byte = 0;
    uint64_t write_byte_cnt = 0;
    uint64_t ori_start_pos = 0;
    uint64_t to_write_byte = new_file_size;
    bool first_block = true;
    bool final_block = false;
    for (auto block_name : fd->block_name) {
      // pass until find the block which has start_offset
      if (passed_byte + fd->block_size[seq] < start_offset) {
        passed_byte += fd->block_size[seq];
        seq++;
        continue;
      } else {
        // If this block is the first one of updating blocks,
        // start position will be start_offset - passed_byte.
        // Otherwise, start position will be 0.
        //uint32_t hash_key = fd->hash_keys[seq];
        if (first_block) {
          first_block = false;
          ori_start_pos = start_offset - passed_byte;
        } else {
          ori_start_pos = 0;
        }
        // write length means the lenght which should be repliaced in THIS block.
        // to_write_byte means remaining total bytes to write
        // If this block is the last one, write_length should be same as to_write_byte
        // Otherwise, write_length should be same as block_size - start position
        uint64_t write_length = fd->block_size[seq] - ori_start_pos;
        if (to_write_byte < write_length) {
          final_block = true;
          write_length = to_write_byte;
        }
        // send message
        string content = sbuffer.substr(write_byte_cnt, write_length);
        BlockUpdate bu(block_name, fd->net_id[seq], ori_start_pos, write_length, fd->replica, content);
        send_message(socket.get(), &bu);
        auto reply = read_reply<Reply> (socket.get());
        if (reply->message != "OK") {
          socket->close();
          cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        } 
        // calculate total write bytes and remaining write bytes
        write_byte_cnt += write_length;
        if (final_block) {
          break;
        }
        to_write_byte -= write_length;
        seq++;
      }
    }
    socket->close();
  }
  cout << "[INFO] " << ori_file_name << " is updated." << endl;
  return EXIT_SUCCESS;
}
// }}}
  /*
  // show {{{
  int DFS::show(int argc, char* argv[]) {
  if (argc < 3) {
  cout << "usage: dfs bl file_name1 file_name2 ..." << endl;
  return EXIT_FAILURE;
  } else {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();
  for (int i=2; i<argc; i++) {
  string file_name = argv[i];
  uint32_t file_hash_key = h(file_name);
  auto socket = connect (file_hash_key);
  FileRequest fr;
  fr.name = file_name;

  send_message (socket.get(), &fr);
  auto fd = read_reply<FileDescription>(socket.get());
  cout << file_name << endl;
  uint64_t seq = 0;
  for (auto block_name : fd->blocks) {
  uint32_t hash_key = fd->hash_keys[seq++]; 
  int which_node = boundaries.get_net_id(hash_key);
  int tmp_node;
  for (int i=0; i<fd->replica; i++) {
  if (i%2 == 1) {
  tmp_node = (which_node + (i+1)/2 + nodes.size()) % nodes.size();
  } else {
  tmp_node = (which_node - i/2 + nodes.size()) % nodes.size();
  }
  string ip = nodes[tmp_node];
  cout << "\t- " << setw(15) << block_name << " : " << setw(15) << ip << endl;
  }
  }
  socket->close(); 
  }
  }
  return EXIT_SUCCESS;
  }
  // }}}
  // pget {{{
  int DFS::pget(int argc, char* argv[]) {
  string file_name = "";
  if (argc < 5) {
  cout << "[INFO] dfs pget file_name start_offset read_byte" << endl;
  return EXIT_FAILURE;
  } else {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  file_name = argv[2];
  uint64_t start_offset = (uint64_t)atoi(argv[3]);
  uint64_t read_byte = (uint64_t)atoi(argv[4]);
  uint32_t file_hash_key = h(file_name);
  auto socket = connect (file_hash_key);
  CheckExist ce;
  ce.name = file_name;
  send_message(socket.get(), &ce);
  auto rep = read_reply<Reply> (socket.get());

  if (rep->message != "TRUE") {
  cerr << "[ERR] " << file_name << " doesn't exist." << endl;
  return EXIT_FAILURE;
  }
  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription> (socket.get());
  socket->close();
  if (start_offset + read_byte > fd->size) {
cerr << "[ERR] Wrong read byte." << endl;
return EXIT_FAILURE;
}
string outfile = "p_" + file_name;
ofstream f(outfile);
uint64_t seq = 0;
uint64_t passed_byte = 0;
uint64_t read_byte_cnt = 0;
uint64_t start_pos = 0;
bool first_block = true;
bool final_block = false;
for (auto block_name : fd->blocks) {
  if (passed_byte + fd->block_size[seq] < start_offset) {
    passed_byte += fd->block_size[seq];
    seq++;
    continue;
  } else {
    uint32_t hash_key = fd->hash_keys[seq++];
    auto tmp_socket = connect(boundaries.get_net_id(hash_key));
    BlockRequest br;
    br.name = block_name; 
    br.hash_key = hash_key; 
    send_message(tmp_socket.get(), &br);
    auto msg = read_reply<BlockInfo>(tmp_socket.get());
    string block_content = msg->content;
    if (first_block) {
      first_block = false;
      start_pos = start_offset - passed_byte;
    } else {
      start_pos = 0;
    }
    uint64_t read_length = block_content.length();
    if (read_byte_cnt + read_length > read_byte) {
      final_block = true;
      read_length = read_byte - read_byte_cnt;
    }
    string sub_str = msg->content.substr(start_pos, read_length);
    f << sub_str;
    read_byte_cnt += sub_str.length();
    tmp_socket->close();
    if (final_block) {
      break;
    }
  }
}
f.close();
}
cout << "[INFO] " << file_name << " is read." << endl;
return EXIT_SUCCESS;
}
// }}}
// append {{{
int DFS::append(int argc, char* argv[]) {
  string ori_file_name = "";
  if (argc < 4) { // argument count check
    cout << "[INFO] dfs append original_file new_file1 new_file2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();
    ori_file_name = argv[2];

    for (int i=3; i<argc; i++) {
      string new_file_name = argv[i];
      uint32_t file_hash_key = h(ori_file_name);
      auto socket = connect(file_hash_key);
      CheckExist ce;
      ce.name = ori_file_name;
      send_message(socket.get(), &ce);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message != "TRUE") { // exist check
        cerr << "[ERR] " << ori_file_name << " doesn't exist." << endl;
        return EXIT_FAILURE;
      }
      FileRequest fr;
      fr.name = ori_file_name;

      ifstream myfile(new_file_name);
      myfile.seekg(0, myfile.end);
      uint64_t new_file_size = myfile.tellg();
      if (new_file_size <= 0) { // input size check
        cerr << "[ERR] " << new_file_name << " size should be greater than 0." << endl;
        return EXIT_FAILURE;
      }

      // start normal append procedure
      send_message(socket.get(), &fr);
      auto fd = read_reply<FileDescription> (socket.get());

      uint64_t seq = fd->blocks.size()-1; // last block
      uint64_t ori_start_pos = 0; // original file's start position (in last block)
      uint64_t to_write_byte = new_file_size;
      uint64_t write_byte_cnt = 0;
      bool update_block = true; // 'false' for append
      bool new_block = false;
      uint32_t hash_key = fd->hash_keys[seq];
      uint64_t write_length = 0;
      uint64_t start = 0;
      uint64_t end = 0;
      uint64_t block_size = 0;

      while (to_write_byte > 0) { // repeat until to_write_byte == 0
        if (update_block == true) { 
          ori_start_pos = fd->block_size[seq];
          if (BLOCK_SIZE - ori_start_pos > to_write_byte) { // can append within original block
            myfile.seekg(start + to_write_byte, myfile.beg);
          } else { // can't write whole contents in one block
            myfile.seekg(start + BLOCK_SIZE - ori_start_pos - 1, myfile.beg);
            new_block = true;
            while (1) {
              if (myfile.peek() =='\n' || myfile.tellg() == 0) {
                break;
              } else {
                myfile.seekg(-1, myfile.cur);
              }
            }
            if (myfile.tellg() <= 0) {
              update_block = false;
            }
          }
        }
        if (update_block == true) { // update block
          write_length = myfile.tellg();
          write_length -= start;
          myfile.seekg(start, myfile.beg);
          char *buffer = new char[write_length+1];
          bzero(buffer, write_length+1);
          myfile.read(buffer, write_length);
          string sbuffer(buffer);
          delete[] buffer;
          BlockUpdate bu;
          bu.name = fd->blocks[seq];
          bu.file_name = ori_file_name;
          bu.seq = seq;
          bu.replica = fd->replica;
          bu.hash_key = hash_key;
          bu.pos = ori_start_pos;
          bu.len = write_length;
          bu.content = sbuffer;
          bu.size = ori_start_pos + write_length;
          auto tmp_socket = connect(boundaries.get_net_id(file_hash_key));
          send_message(tmp_socket.get(), &bu);
          auto reply = read_reply<Reply> (tmp_socket.get());
          tmp_socket->close();
          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          } 
          // calculate total write bytes and remaining write bytes
          to_write_byte -= write_length;
          write_byte_cnt += write_length;
          start += write_length;
          if (new_block == true) { 
            update_block = false;
          }
        } else { // append block
          // make new block
          seq++;
          int which_node = ((file_hash_key % NUM_NODES) + seq) % NUM_NODES;
          start = write_byte_cnt;
          end = start + BLOCK_SIZE -1;
          BlockInfo block_info;

          if (end < to_write_byte) {
            // not final block
            myfile.seekg(end, myfile.beg);
            while (1) {
              if (myfile.peek() =='\n') {
                break;
              } else {
                myfile.seekg(-1, myfile.cur);
                end--;
              }
            }
          } else {
            end = start + to_write_byte;
          }
          myfile.seekg(start, myfile.beg);
          block_size = (uint64_t) end - start;
          write_length = block_size;
          char *buffer = new char[block_size+1];
          bzero(buffer, block_size+1);
          myfile.read(buffer, block_size);
          string sbuffer(buffer);
          delete[] buffer;
          myfile.seekg(start, myfile.beg);
          block_info.content = sbuffer;

          block_info.name = ori_file_name + "_" + to_string(seq);
          block_info.file_name = ori_file_name;
          block_info.hash_key = boundaries.random_within_boundaries(which_node);
          block_info.seq = seq;
          block_info.size = block_size;
          block_info.type = static_cast<int>(FILETYPE::Normal);
          block_info.replica = replica;
          block_info.node = nodes[which_node];
          block_info.l_node = nodes[(which_node-1+NUM_NODES)%NUM_NODES];
          block_info.r_node = nodes[(which_node+1+NUM_NODES)%NUM_NODES];
          block_info.check_commit = 1;

          send_message(socket.get(), &block_info);
          auto reply = read_reply<Reply> (socket.get());

          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          } 
          to_write_byte -= write_length;
          write_byte_cnt += write_length;
          if (to_write_byte == 0) { 
            break;
          }
          start = end;
          end = start + BLOCK_SIZE - 1;
          which_node = (which_node + 1) % NUM_NODES;
        }
      }
      FileUpdate fu;
      fu.name = ori_file_name;
      fu.num_block = seq+1;
      fu.size = fd->size + new_file_size;
      send_message(socket.get(), &fu);
      auto reply = read_reply<Reply> (socket.get());
      myfile.close();
      socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to append file. Details: " << reply->details << endl;
        return EXIT_FAILURE;
      }
      cout << "[INFO] " << argv[i] << " is appended." << endl;
    }
  }
  return EXIT_SUCCESS;
}
// }}}
// sput {{{
int DFS::sput(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "[INFO] dfs sput file_1, file_2, ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();
    FILETYPE type = FILETYPE::Slice;

    for (int i=2; i<argc; i++) {
      string file_name = argv[i];
      CheckExist ce;
      ce.name = file_name;
      uint32_t file_hash_key = h(file_name);
      auto socket = connect(file_hash_key);
      send_message(socket.get(), &ce);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message == "TRUE") {
        cerr << "[ERR] " << file_name << " already exists." << endl;
        continue;
      }

      int which_node = file_hash_key % NUM_NODES;
      int fd = open(file_name.c_str(), 0);

      __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in | std::ios::binary); // 1
      istream myfile(&filebuf);

      FileInfo file_info;
      SliceInfo slice_info;
      MdlistInfo mdlist_info;
      file_info.name = file_name;
      file_info.hash_key = file_hash_key;
      file_info.type = static_cast<int>(type);
      file_info.replica = replica;
      myfile.seekg(0, myfile.end);
      uint64_t file_size = myfile.tellg();
      myfile.seekg(0, myfile.beg);
      file_info.size = file_size;

      block_info.name = file_name + "_0";
      block_info.file_name = file_name;
      block_info.hash_key = boundaries.random_within_boundaries(which_node);
      block_info.seq = 0;
      block_info.size = file_size;
      block_info.type = static_cast<int>(FILETYPE::Normal);
      block_info.replica = replica;
      block_info.node = nodes[which_node];
      block_info.l_node = nodes[(which_node-1+NUM_NODES)%NUM_NODES];
      block_info.r_node = nodes[(which_node+1+NUM_NODES)%NUM_NODES];
      block_info.check_commit = 1;

      block_info.content.assign((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>());
      posix_fadvise(fd, file_size, file_size, POSIX_FADV_WILLNEED);

      send_message(socket.get(), &block_info);
      auto reply = read_reply<Reply> (socket.get());

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return EXIT_FAILURE;
      }
      file_info.num_block = 1;
      send_message(socket.get(), &file_info);
      reply = read_reply<Reply> (socket.get());
      close(fd);
      socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return EXIT_FAILURE;
      } 
      cout << "[INFO] " << argv[i] << " is uploaded." << endl;
    }
    return EXIT_SUCCESS;
  }
}
// }}}
// sget {{{
int DFS::sget(int argc, char* argv[]) {
  return 0;
}
// }}}
// yank {{{
int DFS::yank(int argc, char* argv[]) {
  return 0;
}
// }}}
*/
// paste {{{
/*
   int DFS::paste(int argc, char* argv[]) {
   string ori_file_name = "";
   if (argc < 5) {
   cout << "[INFO] dfs paste original_file new_file start_offset" << endl;
   return EXIT_FAILURE;
   } else {
   Histogram boundaries(NUM_NODES, 0);
   boundaries.initialize();

   ori_file_name = argv[2];
   string new_file_name = argv[3];
   uint64_t start_offset = (uint64_t)atoi(argv[4]);
   uint32_t file_hash_key = h(ori_file_name);
   auto socket = connect (file_hash_key);
   bool file_exist = false;
   CheckExist ce;
   ce.name = ori_file_name;
   send_message(socket.get(), &ce);
   auto rep = read_reply<Reply> (socket.get());

   if (rep->message == "TRUE") {
   file_exist = true;
   }

   if (file_exist == true) {
   FileRequest fr;
   fr.name = ori_file_name;

   ifstream myfile(new_file_name);
   myfile.seekg(0, myfile.end);
   uint64_t new_file_size = myfile.tellg();

   send_message(socket.get(), &fr);
   auto fd = read_reply<FileDescription> (socket.get());
   socket->close();
   if (start_offset + new_file_size > fd->size) {
   cerr << "[ERR] Wrong file size." << endl;
   return EXIT_FAILURE;
   }
   myfile.seekg(0, myfile.beg);
   char *buffer = new char[new_file_size];
   myfile.read(buffer, new_file_size);
   string sbuffer(buffer);
   delete[] buffer;
   myfile.close();

   int seq = 0;
   uint64_t passed_byte = 0;
   uint64_t write_byte_cnt = 0;
   uint32_t ori_start_pos = 0;
   uint32_t to_write_byte = new_file_size;
   bool first_block = true;
   bool final_block = false;
   for (auto block_name : fd->blocks) {
// pass until find the block which has start_offset
if (passed_byte + fd->block_size[seq] < start_offset) {
passed_byte += fd->block_size[seq];
seq++;
continue;
} else {
// If this block is the first one of updating blocks,
// start position will be start_offset - passed_byte.
// Otherwise, start position will be 0.
uint32_t hash_key = fd->hash_keys[seq];
if (first_block) {
first_block = false;
ori_start_pos = start_offset - passed_byte;
} else {
ori_start_pos = 0;
}
// write length means the lenght which should be repliaced in THIS block.
// to_write_byte means remaining total bytes to write
// If this block is the last one, write_length should be same as to_write_byte
// Otherwise, write_length should be same as block_size - start position
uint32_t write_length = fd->block_size[seq] - ori_start_pos;
if (to_write_byte < write_length) {
  final_block = true;
  write_length = to_write_byte;
}
// send message
BlockUpdate bu;
bu.name = block_name; 
bu.file_name = ori_file_name;
bu.seq = seq;
bu.replica = fd->replica; 
bu.hash_key = hash_key; 
bu.pos = ori_start_pos;
bu.len = write_length;
bu.content = sbuffer.substr(write_byte_cnt, write_length);
bu.size = fd->block_size[seq];
auto tmp_socket = connect(boundaries.get_net_id(file_hash_key));
send_message(tmp_socket.get(), &bu);
auto reply = read_reply<Reply> (tmp_socket.get());
tmp_socket->close();
if (reply->message != "OK") {
  cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
  return EXIT_FAILURE;
} 
// calculate total write bytes and remaining write bytes
write_byte_cnt += write_length;
if (final_block) {
  break;
}
to_write_byte -= write_length;
seq++;
}
}
}
cout << "[INFO] " << ori_file_name << " is updated." << endl;
return EXIT_SUCCESS;
}
*/
// }}}
}
