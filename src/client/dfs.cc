// Headers {{{
#include "dfs.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockupdate.hh"
#include "../messages/fileexist.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/blockdel.hh"
#include "../messages/filedescription.hh"
#include "../messages/reply.hh"
#include "../messages/blockrequest.hh"
#include "../common/context.hh"
#include "../common/hash.hh"
#include "../common/histogram.hh"
#include "../messages/factory.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <ext/stdio_filebuf.h>
#include <sstream>
#include "../common/context_singleton.hh"

using namespace std;
using namespace eclipse;
using namespace boost::archive;
using namespace eclipse::messages;
// }}}

namespace velox {

enum class FILETYPE {
  Normal = 0x0,
  App    = 0x1,
  Idata  = 0x2
};

// Constructors and misc {{{
DFS::DFS() { }

void DFS::load_settings() {
  BLOCK_SIZE = context.settings.get<int>("filesystem.block");
  NUM_NODES = context.settings.get<vector<string>>("network.nodes").size();
  replica = context.settings.get<int>("filesystem.replica");
  port = context.settings.get<int> ("network.ports.client");
  nodes = context.settings.get<vector<string>>("network.nodes");
}

unique_ptr<tcp::socket> DFS::connect(uint32_t hash_value) { 
  auto socket = make_unique<tcp::socket>(context.io);
  string host = nodes[hash_value % nodes.size()];
  tcp::resolver resolver(context.io);
  tcp::resolver::query query(host, to_string(port));
  tcp::resolver::iterator it(resolver.resolve(query));
  auto ep = make_unique<tcp::endpoint>(*it);
  socket->connect(*ep);
  return socket;
}

bool DFS::fexists(std::string filename) {
  ifstream ifile(filename);
  return ifile;
}
// }}}
// put {{{
int DFS::put(vec_str input) {
  if (input.size() < 3) {
    cout << "[INFO] dfs put file_1 file_2 ..." << endl;
    return EXIT_FAILURE;

  } else {
    vector<char> chunk(BLOCK_SIZE);
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    string op = input[2];
    FILETYPE type = FILETYPE::Normal;
    uint32_t i = 2;
    if (op.compare("-b") == 0) {
      replica = NUM_NODES;
      type = FILETYPE::App;
      i++;
    }
    for (; i < input.size(); i++) {
      string file_name = input[i];	
      if (!this->fexists(file_name)) {
        cerr << "[ERR] " << file_name << " does not exist." << endl;
        continue;
      }
      FileExist fe;
      fe.name = file_name;
      uint32_t file_hash_key = h(file_name);
      auto socket = connect(file_hash_key);
      send_message(socket.get(), &fe);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message == "TRUE") {
        cerr << "[ERR] " << file_name << " already exists." << endl;
        continue;
      }

      int which_server = file_hash_key % NUM_NODES;
      int fd = open(file_name.c_str(), 0);

      __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in | std::ios::binary); // 1
      istream myfile(&filebuf);
      uint64_t start = 0;
      uint64_t end = start + BLOCK_SIZE - 1;
      uint32_t block_size = 0;
      unsigned int block_seq = 0;

      FileInfo file_info;
      file_info.name = file_name;
      file_info.hash_key = file_hash_key;
      file_info.type = static_cast<unsigned int>(type);
      file_info.replica = replica;
      myfile.seekg(0, myfile.end);
      file_info.size = myfile.tellg();
      BlockInfo block_info;

      while (1) {
        if (end < file_info.size) {
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
          end = file_info.size;
        }
        block_size = (uint32_t) end - start;
        bzero(chunk.data(), BLOCK_SIZE);
        myfile.seekg(start, myfile.beg);
        block_info.content.reserve(block_size);
        myfile.read(chunk.data(), block_size);
        block_info.content = chunk.data();
        posix_fadvise(fd, end, block_size, POSIX_FADV_WILLNEED);


        block_info.name = file_name + "_" + to_string(block_seq);
        block_info.file_name = file_name;
        block_info.hash_key = boundaries.random_within_boundaries(which_server);
        block_info.seq = block_seq++;
        block_info.size = block_size;
        block_info.type = static_cast<unsigned int>(FILETYPE::Normal);
        block_info.replica = replica;
        block_info.node = nodes[which_server];
        block_info.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
        block_info.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
        block_info.is_committed = 1;

        send_message(socket.get(), &block_info);
        auto reply = read_reply<Reply> (socket.get());


        if (reply->message != "OK") {
          cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          return EXIT_FAILURE;
        }
        if (end >= file_info.size) {
          break;
        }
        start = end;
        end = start + BLOCK_SIZE - 1;
        which_server = (which_server + 1) % NUM_NODES;
      }

      file_info.num_block = block_seq;
      send_message(socket.get(), &file_info);
      auto reply = read_reply<Reply> (socket.get());
      close(fd);
      socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return EXIT_FAILURE;
      } 
      cout << "[INFO] " << file_name << " is uploaded." << endl;
    }
  }
  return EXIT_SUCCESS;
}
// }}}
// load {{{
std::string DFS::load(std::string file) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  string file_name = file;
  uint32_t file_hash_key = h(file_name);
  auto socket = connect (file_hash_key);
  FileExist fe;
  fe.name = file_name;
  send_message(socket.get(), &fe);
  auto rep = read_reply<Reply> (socket.get());

  if (rep->message != "TRUE") {
    cerr << "[ERR] " << file_name << " doesn't exist." << endl;
    return "";
  }

  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription> (socket.get());

  std::string output;
  socket->close();
  int block_seq = 0;
  for (auto block_name : fd->blocks) {
    uint32_t hash_key = fd->hash_keys[block_seq++];
    auto tmp_socket = connect(boundaries.get_index(hash_key));
    BlockRequest br;
    br.name = block_name; 
    br.hash_key = hash_key; 
    send_message(tmp_socket.get(), &br);
    auto msg = read_reply<BlockInfo>(tmp_socket.get());
    output += msg->content;
    tmp_socket->close();
  }
  socket->close();
  return output;
}
// }}} 
// get {{{
int DFS::get(vec_str argv) {
  if (argv.size() < 3) {
    cout << "[INFO] dfs get file_1 file_2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    for (uint32_t i = 2; i < argv.size(); i++) {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      auto socket = connect (file_hash_key);
      FileExist fe;
      fe.name = file_name;
      send_message(socket.get(), &fe);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message != "TRUE") {
        cerr << "[ERR] " << file_name << " doesn't exist." << endl;
        continue;
      }
      FileRequest fr;
      fr.name = file_name;

      send_message(socket.get(), &fr);
      auto fd = read_reply<FileDescription> (socket.get());

      ofstream f(file_name);
      socket->close();
      int block_seq = 0;
      for (auto block_name : fd->blocks) {
        uint32_t hash_key = fd->hash_keys[block_seq++];
        auto tmp_socket = connect(boundaries.get_index(hash_key));
        BlockRequest br;
        br.name = block_name; 
        br.hash_key = hash_key; 
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
  return EXIT_SUCCESS;
}
// }}}
// cat {{{
int DFS::cat(vec_str argv) {
  if (argv.size() < 3) {
    cout << "[INFO] dfs cat file_1 file_2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    for (uint32_t i = 2; i < argv.size(); i++) {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      auto socket = connect (file_hash_key % NUM_NODES);
      FileExist fe;
      fe.name = file_name;
      send_message(socket.get(), &fe);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message != "TRUE") {
        cerr << "[ERR] " << file_name << " doesn't exist." << endl;
        continue;
      }
      FileRequest fr;
      fr.name = file_name;

      send_message (socket.get(), &fr);
      auto fd = read_reply<FileDescription> (socket.get());

      socket->close();
      int block_seq = 0;
      for (auto block_name : fd->blocks) {
        uint32_t hash_key = fd->hash_keys[block_seq++];
        auto tmp_socket = connect(boundaries.get_index(hash_key));
        BlockRequest br;
        br.name = block_name; 
        br.hash_key = hash_key; 
        send_message(tmp_socket.get(), &br);
        auto msg = read_reply<BlockInfo>(tmp_socket.get());
        cout << msg->content;
        tmp_socket->close();
      }
      socket->close();
    }
  }
  return EXIT_SUCCESS;
}
// }}}
// ls {{{
int DFS::ls(vec_str argv) {
  const uint32_t KB = 1024;
  const uint32_t MB = 1024 * 1024;
  const uint64_t GB = (uint64_t) 1024 * 1024 * 1024;
  const uint64_t TB = (uint64_t) 1024 * 1024 * 1024 * 1024;
  const uint64_t PB = (uint64_t) 1024 * 1024 * 1024 * 1024 * 1024;
  const uint32_t K = 1000;
  const uint32_t M = 1000 * 1000;
  const uint64_t G = (uint64_t) 1000 * 1000 * 1000;
  const uint64_t T = (uint64_t) 1000 * 1000 * 1000 * 1000;
  const uint64_t P = (uint64_t) 1000 * 1000 * 1000 * 1000 * 1000;
  vector<FileInfo> total; 
  string op = "";
  if (argv.size() >= 3) {
    op = argv[2];
  }
  for (unsigned int net_id=0; net_id<NUM_NODES; net_id++) {
    FileList file_list;
    auto socket = connect(net_id);
    send_message(socket.get(), &file_list);
    auto file_list_reply = read_reply<FileList>(socket.get());
    std::copy(file_list_reply->data.begin(), file_list_reply->data.end(), back_inserter(total));
  }

  std::sort(total.begin(), total.end(), [] (const FileInfo& a, const FileInfo& b) {
      return (a.name < b.name);
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
      << setw(14) << nodes[fl.hash_key % NUM_NODES]
      << setw(5) << fl.replica
      << endl;
  }
  return EXIT_SUCCESS;
}
// }}}
// rm {{{
int DFS::rm(vec_str argv) {
  if (argv.size() < 3) {
    cout << "[INFO] dfs rm file_1 file_2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    for (uint32_t i = 2; i < argv.size(); i++) {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      auto socket = connect(file_hash_key);
      FileRequest fr;
      fr.name = file_name;

      send_message(socket.get(), &fr);
      auto fd = read_reply<FileDescription>(socket.get());
      socket->close();

      unsigned int block_seq = 0;
      for (auto block_name : fd->blocks) {
        uint32_t block_hash_key = fd->hash_keys[block_seq];
        auto tmp_socket = connect(boundaries.get_index(block_hash_key));
        BlockDel bd;
        bd.name = block_name;
        bd.file_name = file_name;
        bd.hash_key = block_hash_key;
        bd.seq = block_seq++;
        bd.replica = fd->replica;
        send_message(tmp_socket.get(), &bd);
        auto msg = read_reply<Reply>(tmp_socket.get());
        if (msg->message != "OK") {
          cerr << "[ERR] " << block_name << "doesn't exist." << endl;
          return EXIT_FAILURE;
        }
        socket->close();
      }

      FileDel file_del;
      file_del.name = file_name;
      socket = connect(file_hash_key);
      send_message(socket.get(), &file_del);
      auto reply = read_reply<Reply>(socket.get());
      if (reply->message != "OK") {
        cerr << "[ERR] " << file_name << " doesn't exist." << endl;
        return EXIT_FAILURE;
      }
      cout << "[INFO] " << file_name << " is removed." << endl;
    }
    return EXIT_SUCCESS;
  }
}
// }}}
// format {{{
int DFS::format() {
  vector<FileInfo> total;

  for (unsigned int net_id = 0; net_id < NUM_NODES; net_id++) {
    FormatRequest fr;
    auto socket = connect(net_id);
    send_message(socket.get(), &fr);
    auto reply = read_reply<Reply>(socket.get());

    if (reply->message != "OK") {
      cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
      return EXIT_FAILURE;
    } 
  }
  cout << "[INFO] dfs format is done." << endl;
  return EXIT_SUCCESS;
}
// }}}
// show {{{
int DFS::show(vec_str argv) {
  if (argv.size() < 3) {
    cout << "usage: dfs show file_name1 file_name2 ..." << endl;
    return EXIT_FAILURE;

  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();
    for (uint32_t i = 2; i < argv.size(); i++) {
      string file_name = argv[i];
      uint32_t file_hash_key = h(file_name);
      auto socket = connect (file_hash_key);
      FileRequest fr;
      fr.name = file_name;

      send_message (socket.get(), &fr);
      auto fd = read_reply<FileDescription>(socket.get());
      cout << file_name << endl;
      int block_seq = 0;
      for (auto block_name : fd->blocks) {
        uint32_t hash_key = fd->hash_keys[block_seq++]; 
        int which_node = boundaries.get_index(hash_key);
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
int DFS::pget(vec_str argv) {
  string file_name = "";
  if (argv.size() < 5) {
    cout << "[INFO] dfs pget file_name start_offset read_byte" << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    file_name = argv[2];
    uint64_t start_offset = stol(argv[3]);
    uint64_t read_byte = stol(argv[4]);
    uint32_t file_hash_key = h(file_name);
    auto socket = connect (file_hash_key);
    FileExist fe;
    fe.name = file_name;
    send_message(socket.get(), &fe);
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
    int block_seq = 0;
    uint64_t passed_byte = 0;
    uint64_t read_byte_cnt = 0;
    uint32_t start_pos = 0;
    bool first_block = true;
    bool final_block = false;
    for (auto block_name : fd->blocks) {
      if (passed_byte + fd->block_size[block_seq] < start_offset) {
        passed_byte += fd->block_size[block_seq];
        block_seq++;
        continue;
      } else {
        uint32_t hash_key = fd->hash_keys[block_seq++];
        auto tmp_socket = connect(boundaries.get_index(hash_key));
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
        uint32_t read_length = block_content.length();
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
// update {{{
int DFS::update(vec_str argv) {
  string ori_file_name = "";
  if (argv.size() < 5) {
    cout << "[INFO] dfs update original_file new_file start_offset" << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();

    ori_file_name = argv[2];
    string new_file_name = argv[3];
    uint64_t start_offset = stol(argv[4]);
    uint32_t file_hash_key = h(ori_file_name);
    auto socket = connect (file_hash_key);
    FileExist fe;
    fe.name = ori_file_name;
    send_message(socket.get(), &fe);
    auto rep = read_reply<Reply> (socket.get());

    if (rep->message != "TRUE") {
      cerr << "[ERR] " << ori_file_name << " doesn't exist." << endl;
      return EXIT_FAILURE;
    }
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

    int block_seq = 0;
    uint64_t passed_byte = 0;
    uint64_t write_byte_cnt = 0;
    uint32_t ori_start_pos = 0;
    uint32_t to_write_byte = new_file_size;
    bool first_block = true;
    bool final_block = false;
    for (auto block_name : fd->blocks) {
      // pass until find the block which has start_offset
      if (passed_byte + fd->block_size[block_seq] < start_offset) {
        passed_byte += fd->block_size[block_seq];
        block_seq++;
        continue;
      } else {
        // If this block is the first one of updating blocks,
        // start position will be start_offset - passed_byte.
        // Otherwise, start position will be 0.
        uint32_t hash_key = fd->hash_keys[block_seq];
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
        uint32_t write_length = fd->block_size[block_seq] - ori_start_pos;
        if (to_write_byte < write_length) {
          final_block = true;
          write_length = to_write_byte;
        }
        // send message
        BlockUpdate bu;
        bu.name = block_name; 
        bu.file_name = ori_file_name;
        bu.seq = block_seq;
        bu.replica = fd->replica; 
        bu.hash_key = hash_key; 
        bu.pos = ori_start_pos;
        bu.len = write_length;
        bu.content = sbuffer.substr(write_byte_cnt, write_length);
        bu.size = fd->block_size[block_seq];
        auto tmp_socket = connect(boundaries.get_index(file_hash_key));
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
        block_seq++;
      }
    }
  }
  cout << "[INFO] " << ori_file_name << " is updated." << endl;
  return EXIT_SUCCESS;
}
// }}}
// append {{{
int DFS::append(vec_str argv) {
  string ori_file_name = "";
  if (argv.size() < 4) { // argument count check
    cout << "[INFO] dfs append original_file new_file1 new_file2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();
    ori_file_name = argv[2];

    for (uint32_t i = 3; i < argv.size(); i++) {
      string new_file_name = argv[i];
      uint32_t file_hash_key = h(ori_file_name);
      auto socket = connect(file_hash_key);
      FileExist fe;
      fe.name = ori_file_name;
      send_message(socket.get(), &fe);
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

      int block_seq = fd->blocks.size()-1; // last block
      uint32_t ori_start_pos = 0; // original file's start position (in last block)
      uint64_t to_write_byte = new_file_size;
      uint64_t write_byte_cnt = 0;
      bool update_block = true; // 'false' for append
      bool new_block = false;
      uint32_t hash_key = fd->hash_keys[block_seq];
      uint64_t write_length = 0;
      uint64_t start = 0;
      uint64_t end = 0;
      uint32_t block_size = 0;

      while (to_write_byte > 0) { // repeat until to_write_byte == 0
        if (update_block == true) { 
          ori_start_pos = fd->block_size[block_seq];
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
          bu.name = fd->blocks[block_seq];
          bu.file_name = ori_file_name;
          bu.seq = block_seq;
          bu.replica = fd->replica;
          bu.hash_key = hash_key;
          bu.pos = ori_start_pos;
          bu.len = write_length;
          bu.content = sbuffer;
          bu.size = ori_start_pos + write_length;
          auto tmp_socket = connect(boundaries.get_index(file_hash_key));
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
          block_seq++;
          int which_server = ((file_hash_key % NUM_NODES) + block_seq) % NUM_NODES;
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
          block_size = (uint32_t) end - start;
          write_length = block_size;
          char *buffer = new char[block_size+1];
          bzero(buffer, block_size+1);
          myfile.read(buffer, block_size);
          string sbuffer(buffer);
          delete[] buffer;
          myfile.seekg(start, myfile.beg);
          block_info.content = sbuffer;

          block_info.name = ori_file_name + "_" + to_string(block_seq);
          block_info.file_name = ori_file_name;
          block_info.hash_key = boundaries.random_within_boundaries(which_server);
          block_info.seq = block_seq;
          block_info.size = block_size;
          block_info.type = static_cast<unsigned int>(FILETYPE::Normal);
          block_info.replica = replica;
          block_info.node = nodes[which_server];
          block_info.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
          block_info.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
          block_info.is_committed = 1;

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
          which_server = (which_server + 1) % NUM_NODES;
        }
      }
      FileUpdate fu;
      fu.name = ori_file_name;
      fu.num_block = block_seq+1;
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
// push_back {{{
int DFS::push_back(vec_str argv) {
  string ori_file_name = "";
  if (argv.size() < 4) { // argument count check
    cout << "[INFO] dfs append original_file new_file1 new_file2 ..." << endl;
    return EXIT_FAILURE;
  } else {
    Histogram boundaries(NUM_NODES, 0);
    boundaries.initialize();
    ori_file_name = argv[2];

    for (uint32_t i = 3; i < argv.size(); i++) {
      string content= argv[i];
      uint32_t file_hash_key = h(ori_file_name);
      auto socket = connect(file_hash_key);
      FileExist fe;
      fe.name = ori_file_name;
      send_message(socket.get(), &fe);
      auto rep = read_reply<Reply> (socket.get());

      if (rep->message != "TRUE") { // exist check
        cerr << "[ERR] " << ori_file_name << " doesn't exist." << endl;
        return EXIT_FAILURE;
      }
      FileRequest fr;
      fr.name = ori_file_name;

      istringstream myfile (content);
      //ifstream myfile(new_file_name);
      myfile.seekg(0, myfile.end);
      uint64_t new_file_size = myfile.tellg();
      if (new_file_size <= 0) { // input size check
        cerr << "[ERR] " << content << " size should be greater than 0." << endl;
        return EXIT_FAILURE;
      }

      // start normal append procedure
      send_message(socket.get(), &fr);
      auto fd = read_reply<FileDescription> (socket.get());

      int block_seq = fd->blocks.size()-1; // last block
      uint32_t ori_start_pos = 0; // original file's start position (in last block)
      uint64_t to_write_byte = new_file_size;
      uint64_t write_byte_cnt = 0;
      bool update_block = true; // 'false' for append
      bool new_block = false;
      uint32_t hash_key = fd->hash_keys[block_seq];
      uint64_t write_length = 0;
      uint64_t start = 0;
      uint64_t end = 0;
      uint32_t block_size = 0;

      while (to_write_byte > 0) { // repeat until to_write_byte == 0
        if (update_block == true) { 
          ori_start_pos = fd->block_size[block_seq];
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
          bu.name = fd->blocks[block_seq];
          bu.file_name = ori_file_name;
          bu.seq = block_seq;
          bu.replica = fd->replica;
          bu.hash_key = hash_key;
          bu.pos = ori_start_pos;
          bu.len = write_length;
          bu.size = ori_start_pos + write_length;
          bu.content = sbuffer;
          bu.is_header = true;

          send_message(socket.get(), &bu);
          auto reply = read_reply<Reply> (socket.get());
          if (reply->message != "OK") {
            cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
            return EXIT_FAILURE;
          } 

          //bu.is_header = false;

          //auto tmp_socket = connect(boundaries.get_index(hash_key));
          //send_message(tmp_socket.get(), &bu);
          //reply = read_reply<Reply> (tmp_socket.get());
          //tmp_socket->close();
          //if (reply->message != "OK") {
          //  cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
          //  return EXIT_FAILURE;
          //} 
          // calculate total write bytes and remaining write bytes
          to_write_byte -= write_length;
          write_byte_cnt += write_length;
          start += write_length;
          if (new_block == true) { 
            update_block = false;
          }
        } else { // append block
          // make new block
          block_seq++;
          int which_server = ((file_hash_key % NUM_NODES) + block_seq) % NUM_NODES;
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
          block_size = (uint32_t) end - start;
          write_length = block_size;
          char *buffer = new char[block_size+1];
          bzero(buffer, block_size+1);
          myfile.read(buffer, block_size);
          string sbuffer(buffer);
          delete[] buffer;
          myfile.seekg(start, myfile.beg);
          block_info.content = sbuffer;

          block_info.name = ori_file_name + "_" + to_string(block_seq);
          block_info.file_name = ori_file_name;
          block_info.hash_key = boundaries.random_within_boundaries(which_server);
          block_info.seq = block_seq;
          block_info.size = block_size;
          block_info.type = static_cast<unsigned int>(FILETYPE::Normal);
          block_info.replica = replica;
          block_info.node = nodes[which_server];
          block_info.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
          block_info.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
          block_info.is_committed = 1;

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
          which_server = (which_server + 1) % NUM_NODES;
        }
      }
      FileUpdate fu;
      fu.name = ori_file_name;
      fu.num_block = block_seq+1;
      fu.size = fd->size + new_file_size;
      send_message(socket.get(), &fu);
      auto reply = read_reply<Reply> (socket.get());
      //myfile.close();
      socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to append file. Details: " << reply->details << endl;
        return EXIT_FAILURE;
      }
    }
  }
  return EXIT_SUCCESS;
}
/// }}} 
// exists {{{
bool DFS::exists(std::string name) {
  FileExist fe;
  fe.name = name;
  uint32_t hash_key = h(name);
  auto socket = connect(hash_key);
  send_message(socket.get(), &fe);
  auto rep = read_reply<Reply> (socket.get());
  socket->close();

  return (rep->message == "TRUE");
}
// }}}
// touch {{{
bool DFS::touch(std::string name) {
  if (exists(name))
    return false;

  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  int which_server = h(name) % NUM_NODES;

  BlockInfo block_info;
  block_info.name = name + "_0";
  block_info.file_name = name;
  block_info.hash_key = boundaries.random_within_boundaries(which_server);
  block_info.seq = 0;
  block_info.size = 0;
  block_info.type = static_cast<unsigned int>(FILETYPE::Normal);
  block_info.replica = replica;
  block_info.node = nodes[which_server];
  block_info.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
  block_info.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
  block_info.is_committed = 1;
  block_info.content = "NOOP";

  auto socket = connect(h(name));
  send_message(socket.get(), &block_info);
  auto reply = read_reply<Reply> (socket.get());

  FileInfo file_info;
  file_info.name = name;
  file_info.hash_key = h(name);
  file_info.type = static_cast<unsigned int>(FILETYPE::Normal);
  file_info.replica = replica;
  file_info.size = 0;
  file_info.num_block = 1;

  send_message(socket.get(), &file_info);
  reply = read_reply<Reply> (socket.get());
  socket->close();

  return (reply->message == "OK");
}
// }}}
}
