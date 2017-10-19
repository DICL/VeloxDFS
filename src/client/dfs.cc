// Headers {{{
#include "dfs.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/fileexist.hh"
#include "../messages/filedescription.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/reply.hh"
#include "../messages/blockrequest.hh"
#include "../common/context.hh"
#include "../common/hash.hh"
#include "../common/histogram.hh"
#include "../common/block.hh"
#include "../common/blockmetadata.hh"
#include "../messages/factory.hh"
#include "../messages/IOoperation.hh"
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <ext/stdio_filebuf.h>
#include <algorithm>
#include <stack>
#include <future>
#include "../common/context_singleton.hh"

using namespace std;
using namespace eclipse;
using namespace boost::archive;
using namespace eclipse::messages;
using boost::asio::ip::tcp;
// }}}

namespace velox {

enum class FILETYPE {
  Normal = 0x0,
  App    = 0x1,
  Idata  = 0x2
};

// Static functions {{{
//
static unique_ptr<tcp::socket> connect(uint32_t hash_value) { 
  auto nodes = GET_VEC_STR("network.nodes");
  auto port = GET_INT("network.ports.client");

  auto socket = make_unique<tcp::socket>(context.io);
  string host = nodes[hash_value % nodes.size()];
  tcp::resolver resolver(context.io);
  tcp::resolver::query query(host, to_string(port));
  tcp::resolver::iterator it(resolver.resolve(query));
  auto ep = make_unique<tcp::endpoint>(*it);
  socket->connect(*ep);
  return socket;
}

unique_ptr<FileDescription> get_file_description
(std::function<unique_ptr<tcp::socket>(uint32_t)> connect, std::string& fname, bool only_metadata) {

  uint32_t file_hash_key = h(fname);
  auto socket = connect(file_hash_key);

  FileExist fe;
  fe.name = fname;
  send_message(socket.get(), &fe);
  auto rep = read_reply<Reply> (socket.get());

  if (rep->message != "TRUE") {
    cerr << "[ERR] " << fname << " doesn't exist." << endl;
    return nullptr;
  }

  FileRequest fr;
  fr.name = fname;

  send_message(socket.get(), &fr);
  unique_ptr<FileDescription> fd = (read_reply<FileDescription> (socket.get()));
  socket->close();

  return fd;
}

unique_ptr<FileDescription> get_file_description
  (std::function<unique_ptr<tcp::socket>(uint32_t)> connect, std::string& fname) {

  return get_file_description(connect, fname, false);
}

static bool file_exists_local(std::string filename) {
  ifstream ifile(filename);
  return ifile.good();
}
// }}}
// Constructors and misc {{{
DFS::DFS() { 
  BLOCK_SIZE = context.settings.get<int>("filesystem.block");
  NUM_NODES = context.settings.get<vector<string>>("network.nodes").size();
  replica = context.settings.get<int>("filesystem.replica");
  nodes = context.settings.get<vector<string>>("network.nodes");
}

// }}}
// upload {{{
int DFS::upload(std::string file_name, bool is_binary) {
  FILETYPE type = FILETYPE::Normal;
  int replica = GET_INT("filesystem.replica");

  if (is_binary) {
    replica = NUM_NODES;
    type = FILETYPE::App;
  }

  uint32_t file_hash_key = h(file_name);
  if (not file_exists_local(file_name)) {
    cerr << "[ERR] " << file_name << " cannot be found in your machine." << endl;
    return EXIT_FAILURE;
  }

  //! Does the file exists
  if (this->exists(file_name)) {
    cerr << "[ERR] " << file_name << " already exists in VeloxDFS." << endl;
    return EXIT_FAILURE;
  }

  //! Insert the file
  int fd = open(file_name.c_str(), 0);

  __gnu_cxx::stdio_filebuf<char> filebuf(fd, std::ios::in | std::ios::binary);
  istream myfile(&filebuf);

  FileInfo file_info;
  file_info.name = file_name;
  file_info.hash_key = file_hash_key;
  file_info.type = static_cast<unsigned int>(type);
  file_info.replica = replica;
  myfile.seekg(0, ios_base::end);
  file_info.size = myfile.tellg();

  //! Send file to be submitted;
  auto socket = connect(file_hash_key);
  send_message(socket.get(), &file_info);

  //! Get information of where to send the file
  auto description = read_reply<FileDescription>(socket.get());
  socket->close();

  uint64_t start = 0;
  uint64_t end = start + BLOCK_SIZE - 1;
  uint32_t block_size = 0;
  unsigned int block_seq = 0;

  //! Insert the blocks
  uint32_t i = 0;

  vector<BlockMetadata> blocks_metadata;
  vector<future<bool>> slave_sockets;
  vector<char> chunk(BLOCK_SIZE);
  Histogram boundaries(NUM_NODES, 100);
  boundaries.initialize();

  while (true) {
    if (end < file_info.size) {
      myfile.seekg(start+BLOCK_SIZE-1, ios_base::beg);
      while (myfile.peek() != '\n') {
        myfile.seekg(-1, ios_base::cur);
        end--;
      }
    } else {
      end = file_info.size;
    }
    BlockMetadata metadata;
    Block block;

    block_size = (uint32_t) end - start;
    bzero(chunk.data(), BLOCK_SIZE);
    myfile.seekg(start, myfile.beg);
    block.second.reserve(block_size);
    myfile.read(chunk.data(), block_size);
    block.second = move(chunk.data());
    posix_fadvise(fd, end, block_size, POSIX_FADV_WILLNEED);

    //! Load block metadata info
    int which_server = ((file_hash_key % NUM_NODES) + i) % NUM_NODES;
    block.first = metadata.name = file_name + string("_") + to_string(i);
    metadata.file_name = file_name;
    metadata.hash_key = boundaries.random_within_boundaries(which_server);
    metadata.seq = block_seq++;
    metadata.size = block_size;
    metadata.type = static_cast<unsigned int>(FILETYPE::Normal);
    metadata.replica = replica;
    metadata.node = nodes[which_server];
    metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
    metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
    metadata.is_committed = 1;

    blocks_metadata.push_back(metadata);

    IOoperation io_ops;
    io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;
    io_ops.block = move(block);

    auto socket = connect(boundaries.get_index(metadata.hash_key));
    send_message(socket.get(), &io_ops);

    auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
        auto reply = read_reply<Reply> (socket.get());
        socket->close();

        if (reply->message != "TRUE") {
        cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
        return false;
        }

        return true;
        }, move(socket));

    slave_sockets.push_back(move(future));

    if (end >= file_info.size) {
      break;
    }
    start = end;
    end = start + BLOCK_SIZE - 1;
    i++;
  }

  for (auto& future: slave_sockets)
    future.get();

  file_info.num_block = block_seq;
  file_info.blocks_metadata = blocks_metadata;
  file_info.uploading = 0;

  socket = connect(file_hash_key);
  send_message(socket.get(), &file_info);
  auto reply = read_reply<Reply> (socket.get());

  if (reply->message != "TRUE") {
    cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
    return EXIT_FAILURE;
  }

  socket->close();
  close(fd);

  cout << "[INFO] " << file_name << " is uploaded." << endl;
  return EXIT_SUCCESS;
}
// }}}
// read_block {{{
int read_block(model::metadata& md, std::string block_name, char* out) {
  string disk_path = GET_STR("path.scratch");
  uint64_t cursor = 0;

  auto it = std::find_if(md.block_data.begin(), md.block_data.end(), [block_name] (auto& block) {
      return block_name == block.name;
      });

  if (it != md.block_data.end()) {
    model::block_metadata bm = *it;
    size_t total_size = bm.size;
    out = new char[total_size];

    for (auto& path_of_chunk : bm.chunks_path) {
      string file_path = disk_path + string("/") + path_of_chunk;
      ifstream ifs;
      ifs.open(file_path, ios::binary | ios::in);

      uint32_t file_size = (uint32_t)ifs.tellg();
      ifs.seekg(0L, ios::beg);

      ifs.read(&out[cursor], file_size);
      ifs.close();

      cursor += file_size;
    }
  }

  return cursor;
}
//}}}
// download {{{
int DFS::download(std::string file_name) {
  Histogram boundaries(NUM_NODES, 100);
  boundaries.initialize();

  //! Does the file exists
  if (not this->exists(file_name)) {
    cerr << "[ERR] " << file_name << " already doesn't exists in VeloxDFS." << endl;
    return EXIT_FAILURE;
  }

  uint32_t file_hash_key = h(file_name);
  auto socket = connect (file_hash_key);

  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription> (socket.get());
  socket->close();

  ofstream file;
  file.rdbuf()->pubsetbuf(0, 0);      //! No buffer
  file.open(file_name, ios::binary);

  for (uint32_t i = 0; i < fd->blocks.size(); i++) {
    IOoperation io_ops;
    io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
    io_ops.block.first = fd->blocks[i];

    auto slave_socket = connect(boundaries.get_index(fd->hash_keys[i]));
    send_message(slave_socket.get(), &io_ops);
    auto msg = read_reply<IOoperation>(slave_socket.get());

    file.write(msg->block.second.c_str(), msg->block.second.length());
    slave_socket->close();
  }

  file.close();

  return EXIT_SUCCESS;
}
// }}}
// read_all {{{
std::string DFS::read_all(std::string file) {
  Histogram boundaries(NUM_NODES, 100);
  boundaries.initialize();

  auto fd = get_file_description(
    std::bind(&connect, std::placeholders::_1), file
  );
  if(fd == nullptr) return "";

  std::string output;
  int index = 0;

  for (auto block_name : fd->blocks) {
    IOoperation io_ops;
    io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
    io_ops.block.first = fd->blocks[index];

    auto slave_socket = connect(boundaries.get_index(fd->hash_keys[index]));
    send_message(slave_socket.get(), &io_ops);
    auto msg = read_reply<IOoperation>(slave_socket.get());
    output += msg->block.second;
    slave_socket->close();
    index++;
  }

  return output;
}
// }}} 
// remove {{{
int DFS::remove(std::string file_name) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  uint32_t file_hash_key = h(file_name);
  auto socket = connect(file_hash_key);
  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription>(socket.get());
  //socket->close();

  unsigned int block_seq = 0;
  for (auto block_name : fd->blocks) {
    uint32_t block_hash_key = fd->hash_keys[block_seq++];
    auto tmp_socket = connect(boundaries.get_index(block_hash_key));
    IOoperation io_ops;
    io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_DELETE;
    io_ops.block.first = block_name;

    send_message(tmp_socket.get(), &io_ops);
    auto msg = read_reply<Reply>(tmp_socket.get());
    if (msg->message != "TRUE") {
      cerr << "[ERR] " << block_name << "doesn't exist." << endl;
      return EXIT_FAILURE;
    }
  }

  FileDel file_del;
  file_del.name = file_name;
  //socket = connect(file_hash_key);
  send_message(socket.get(), &file_del);
  auto reply = read_reply<Reply>(socket.get());
  if (reply->message != "OK") {
    cerr << "[ERR] " << file_name << " doesn't exist." << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
// }}}
// format {{{
int DFS::format() {
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
// pget {{{
//! @deprecated
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

    auto fd = get_file_description(
      std::bind(&connect, std::placeholders::_1), file_name
    );
    if(fd == nullptr) return EXIT_FAILURE;

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
    uint32_t start_offset = stol(argv[4]);

    ifstream myfile(new_file_name);
    myfile.seekg(0, myfile.end);
    uint32_t new_file_size = myfile.tellg();

    char *buffer = new char[new_file_size];

    myfile.seekg(0, myfile.beg);
    myfile.read(buffer, new_file_size);

    uint64_t written_bytes = write(ori_file_name, buffer, start_offset, new_file_size);
    int ret = (written_bytes > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    
    delete[] buffer;
    myfile.close();

    return ret;
  }
}
// }}}
// append {{{
//! @todo fix implementation
int DFS::append(string file_name, string buf) {
  string ori_file_name = file_name; 
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

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

  istringstream myfile (buf);
  //ifstream myfile(new_file_name);
  myfile.seekg(0, myfile.end);
  uint64_t new_file_size = myfile.tellg();
  if (new_file_size <= 0) { // input size check
    cerr << "[ERR] " << buf << " size should be greater than 0." << endl;
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
  vector<BlockMetadata> blocks_metadata;

  while (to_write_byte > 0) { // repeat until to_write_byte == 0
    BlockMetadata metadata;
    Block block;
    IOoperation io_ops;

    if (update_block == true) { 
      ori_start_pos = fd->block_size[block_seq];
      if (BLOCK_SIZE - ori_start_pos > to_write_byte) { // can append within original block
        myfile.seekg(start + to_write_byte, myfile.beg);
      } else { // can't write whole bufs in one block
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

      metadata.name = fd->blocks[block_seq];
      metadata.file_name = ori_file_name;
      metadata.seq = block_seq;
      metadata.replica = fd->replica;
      metadata.hash_key = hash_key;
      metadata.size = ori_start_pos + write_length;
      metadata.l_node = "0";
      metadata.r_node = "0";
      metadata.is_committed = 1;

      blocks_metadata.push_back(metadata);

      block.first = metadata.name;
      block.second = move(sbuffer);

      io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;
      io_ops.block = move(block);
      io_ops.pos = ori_start_pos;
      io_ops.length = write_length;

      auto block_server = connect(boundaries.get_index(metadata.hash_key));
      send_message(block_server.get(), &io_ops);
      auto reply = read_reply<Reply> (block_server.get());
      block_server->close();
      if (reply->message != "TRUE") {
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

      metadata.name = ori_file_name + "_" + to_string(block_seq);
      metadata.file_name = ori_file_name;
      metadata.hash_key = boundaries.random_within_boundaries(which_server);
      metadata.seq = block_seq;
      metadata.size = block_size;

      metadata.replica = replica;
      metadata.node = nodes[which_server];
      metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
      metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
      metadata.is_committed = 1;
      
      blocks_metadata.push_back(metadata);

      IOoperation io_ops;
      io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;
      io_ops.block.first = metadata.name;
      io_ops.block.second = move(sbuffer);

      auto block_server = connect(boundaries.get_index(metadata.hash_key));
      send_message(block_server.get(), &io_ops);
      auto reply = read_reply<Reply> (block_server.get());
      block_server->close();

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
  fu.blocks_metadata = blocks_metadata;

  send_message(socket.get(), &fu);
  auto reply = read_reply<Reply> (socket.get());
  socket->close();

  if (reply->message != "OK") {
    cerr << "[ERR] Failed to append file. Details: " << reply->details << endl;
    return EXIT_FAILURE;
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
bool DFS::touch(std::string file_name) {
  if (exists(file_name))
    return false;

  Histogram boundaries(NUM_NODES, 100);
  boundaries.initialize();

  FILETYPE type = FILETYPE::Normal;

  //! If it is an app to be upload :TODO:
  /*
  if (is_binary) {
    replica = NUM_NODES;
    type = FILETYPE::App;
  }
  */

  uint32_t file_hash_key = h(file_name);

  //! Insert the file
  FileInfo file_info;
  file_info.name = file_name;
  file_info.hash_key = file_hash_key;
  file_info.type = static_cast<unsigned int>(type);
  file_info.replica = replica;
  file_info.size = 0ul;

  //! Send file to be submitted;
  auto socket = connect(file_hash_key);
  send_message(socket.get(), &file_info);

  //! Get information of where to send the file
  auto description = read_reply<FileDescription>(socket.get());
  socket->close();

  IOoperation io_ops;
  io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;

  int which_server = (description->hash_key % NUM_NODES) % NUM_NODES;

  BlockMetadata metadata;
  metadata.name = file_name + "_" + to_string(0);
  metadata.file_name = file_name;
  metadata.hash_key = boundaries.random_within_boundaries(which_server);
  metadata.seq = 0;
  metadata.size = 0;
  metadata.type = static_cast<unsigned int>(type);
  metadata.replica = replica;
  metadata.node = nodes[which_server];
  metadata.l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
  metadata.r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
  metadata.is_committed = 1;

  Block block;
  block.first = metadata.name;
  block.second = "";

  io_ops.block = move(block);

  socket = connect(boundaries.get_index(metadata.hash_key));
  send_message(socket.get(), &io_ops);

  auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
      auto reply = read_reply<Reply> (socket.get());
      socket->close();

      if (reply->message != "TRUE") {
      cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
      return false;
      }

      return true;
      }, move(socket));

  future.get();

  file_info.num_block = 1;
  file_info.blocks_metadata.push_back(metadata);
  file_info.uploading = 0;

  socket = connect(file_hash_key);
  send_message(socket.get(), &file_info);
  auto reply = read_reply<Reply> (socket.get());

  if (reply->message != "TRUE") {
    cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
    return EXIT_FAILURE;
  }

  socket->close();

  return EXIT_SUCCESS;
}
// }}}
// write {{{
uint64_t DFS::write(std::string& file_name, const char* buf, uint64_t off, uint64_t len) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  //auto fd = get_file_description(
    //std::bind(&connect, *this, std::placeholders::_1), file_name
  //);

  auto socket = connect(h(file_name));

  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = (read_reply<FileDescription> (socket.get()));
  socket->close();
  if(fd == nullptr) return 0;

  off = std::max(0ul, std::min(off, std::max(fd->size, BLOCK_SIZE - 1)));

  //! Insert the blocks
  vector<BlockMetadata> blocks_metadata;
  vector<future<bool>> slave_sockets;

  uint64_t to_write_bytes = len;
  uint64_t written_bytes = 0ul;

  int block_beg_seq = (int) off / BLOCK_SIZE;
  int block_end_seq = (int) (len + off - 1) / BLOCK_SIZE;

  for(int i=block_beg_seq; i<=block_end_seq; i++) {
    BlockMetadata metadata;
    Block block;
    IOoperation io_ops;

    uint64_t pos_to_update, len_to_write;

    if(i < (int)fd->num_block) { // updating exist block
      io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_UPDATE;

      //! Load block metadata info
      int which_server = fd->hash_keys[i] % NUM_NODES;

      pos_to_update = (i == block_beg_seq && fd->block_size[i] > 0) ? (off % BLOCK_SIZE) : 0;
      len_to_write = (fd->block_size[i] == 0) ? std::min(to_write_bytes, BLOCK_SIZE) : std::min((BLOCK_SIZE - pos_to_update), to_write_bytes);

      metadata.name = fd->blocks[i];
      metadata.file_name = file_name;
      metadata.hash_key = fd->hash_keys[i];
      metadata.seq = i;
      metadata.size = std::max(fd->block_size[i], pos_to_update + len_to_write);
      metadata.type = static_cast<unsigned int>(FILETYPE::Normal);
      metadata.replica = fd->replica;
      metadata.node = nodes[which_server];
      metadata.l_node = nodes[(which_server-1+NUM_NODES)%NUM_NODES];
      metadata.r_node = nodes[(which_server+1+NUM_NODES)%NUM_NODES];
      metadata.is_committed = 1;
    }
    else { // creating a new block
      io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_INSERT;

      int which_server = ((fd->hash_key % NUM_NODES) + i) % NUM_NODES;

      pos_to_update = 0;
      len_to_write = std::min(BLOCK_SIZE, to_write_bytes);

      metadata.name = file_name + "_" + to_string(i);
      metadata.file_name = file_name;
      metadata.hash_key = boundaries.random_within_boundaries(which_server);
      metadata.seq = i;
      metadata.size = len_to_write;
      metadata.type = static_cast<unsigned int>(FILETYPE::Normal);
      metadata.replica = fd->replica;
      metadata.node = nodes[which_server];
      metadata.l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
      metadata.r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
      metadata.is_committed = 1;
    }

    blocks_metadata.push_back(metadata);

    string content_str(buf + written_bytes, len_to_write);
    block.first = metadata.name;
    block.second = content_str;

    io_ops.block = move(block);
    io_ops.pos = pos_to_update;
    io_ops.length = len_to_write;

    socket = connect(boundaries.get_index(metadata.hash_key));
    send_message(socket.get(), &io_ops);

    auto future = async(launch::async, [](unique_ptr<tcp::socket> socket) -> bool {
      auto reply = read_reply<Reply> (socket.get());
      socket->close();

      if (reply->message != "TRUE") {
        cerr << "[ERR] Failed to upload block . Details: " << reply->details << endl;
        return false;
      }

      return true;
    }, move(socket));

    slave_sockets.push_back(move(future));

    written_bytes += len_to_write;
    to_write_bytes -= written_bytes;
  }

  for (auto& future: slave_sockets)
    future.get();

  // Update Metadata
  FileUpdate fu;
  fu.name = fd->name;
  fu.num_block = std::max(fd->num_block, (unsigned int)block_end_seq + 1);
  fu.size = std::max(written_bytes + off, fd->size);
  fu.blocks_metadata = blocks_metadata;

  socket = connect(fd->hash_key);
  send_message(socket.get(), &fu);
  auto reply = read_reply<Reply> (socket.get());
  socket->close();

  return written_bytes;
}
// }}}
// read {{{
uint64_t DFS::read(std::string& file_name, char* buf, uint64_t off, uint64_t len) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  uint32_t file_hash_key = h(file_name);
  auto socket = connect(file_hash_key);

  // Get a file from dfs
  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription> (socket.get());

  socket->close();

  if(fd == nullptr) return 0;

  off = std::max(0ul, std::min(off, fd->size));
  if(off >= fd->size) return 0;

  int block_beg_seq = (int) off / BLOCK_SIZE;
  int block_end_seq = (int) (len + off - 1) / BLOCK_SIZE;

  std::string output = "";

  uint64_t remain_len = len;

  // Request blocks
  for(int i=block_beg_seq; i<=block_end_seq; i++) {
    uint32_t hash_key = fd->hash_keys[i];
    auto block_socket = connect(boundaries.get_index(hash_key));

    IOoperation io_ops;
    io_ops.operation = eclipse::messages::IOoperation::OpType::BLOCK_REQUEST;
    io_ops.block.first = fd->blocks[i];
    io_ops.pos = (i == block_beg_seq && fd->block_size[i] > 0) ? (off % fd->block_size[i]) : 0;
    io_ops.length = std::min((fd->block_size[i] - io_ops.pos), remain_len);

    auto slave_socket = connect(boundaries.get_index(fd->hash_keys[i]));
    send_message(slave_socket.get(), &io_ops);
    auto msg = read_reply<IOoperation>(slave_socket.get());
    output += msg->block.second;
    slave_socket->close();

    remain_len -= io_ops.length;

    // What is it??
    if(io_ops.pos + io_ops.length > fd->block_size[i])
      break;
  }

  strcpy(buf, output.c_str());

  return (uint64_t)output.length();
}
// }}}
// get_metadata {{{
model::metadata DFS::get_metadata(std::string& fname) {
  model::metadata md;

  FileRequest fr;
  fr.name = fname;

  auto socket = connect(h(fname));
  send_message(socket.get(), &fr);
  auto fd = (read_reply<FileDescription> (socket.get()));
  socket->close();

  if(fd != nullptr) {
    md.name = fd->name;
    md.hash_key = fd->hash_key;
    md.size = fd->size;
    md.num_block = fd->n_lblock;
    md.type = fd->type;
    md.replica = fd->replica;
    
    // TODO: They must be removed
    md.blocks = fd->blocks;
    md.hash_keys = fd->hash_keys;
    md.block_size = fd->block_size;
    
    // set block metadata
    for(int i=0; i<(int)fd->num_block; i++) {
      model::block_metadata bdata;
      bdata.name = fd->blocks[i];
      bdata.size = fd->block_size[i];
      bdata.host = fd->block_hosts[i];
      bdata.index = i;
      bdata.file_name = fd->name;

      md.block_data.push_back(bdata);
    }
  }

  return md;
}
// }}}
// get_metadata_optimized {{{
model::metadata DFS::get_metadata_optimized(std::string& fname) {
  model::metadata md;

  FileRequest fr;
  fr.name = fname;
  fr.type = "LOGICAL_BLOCKS";

  auto socket = connect(h(fname));
  send_message(socket.get(), &fr);
  auto fd = (read_reply<FileDescription> (socket.get()));
  socket->close();

  if(fd != nullptr) {
    md.name = fd->name;
    md.hash_key = fd->hash_key;
    md.size = fd->size;
    md.num_block = fd->n_lblock;
    md.type = fd->type;
    md.replica = fd->replica;
    
    // TODO: They must be removed
    md.blocks = fd->blocks;
    md.hash_keys = fd->hash_keys;
    md.block_size = fd->block_size;
    
    // set block metadata
    for (auto& lblock : fd->logical_blocks) {
      model::block_metadata bdata;
      bdata.name      = lblock.name;
      bdata.size      = lblock.size;
      bdata.host      = lblock.host_name;
      bdata.index     = lblock.seq;
      bdata.file_name = lblock.file_name;
      for (auto& py_block : lblock.physical_blocks)
        bdata.chunks_path.push_back(py_block.name);

      md.block_data.push_back(bdata);
    }
  }

  return md;
}
// }}}
// get_metadata_all {{{
vector<model::metadata> DFS::get_metadata_all() {
  vector<FileInfo> total; 

  for (unsigned int net_id=0; net_id<NUM_NODES; net_id++) {
    FileList file_list;
    auto socket = connect(net_id);
    send_message(socket.get(), &file_list);
    auto file_list_reply = read_reply<FileList>(socket.get());
    std::copy(file_list_reply->data.begin(), file_list_reply->data.end(), back_inserter(total));
  }

  vector<model::metadata> metadata_vector;

  for (auto fd : total) {
    model::metadata md;
    md.name = fd.name;
    md.hash_key = fd.hash_key;
    md.size = fd.size;
    md.num_block = fd.num_block;
    md.type = fd.type;
    md.replica = fd.replica;
    metadata_vector.push_back(md);
  }
  
  return move(metadata_vector);
}
// }}}
// file_metadata_append {{{
void DFS::file_metadata_append(std::string name, size_t size, model::metadata& blocks) {
  FileUpdate fu;
  fu.name = name;
  fu.num_block = blocks.blocks.size();
  fu.size = size;
  fu.is_append = true;

  for (size_t i = 0; i < blocks.blocks.size(); i++) {
    BlockMetadata metadata;
    metadata.file_name = name;
    metadata.name = blocks.blocks[i];
    metadata.seq = 0;
    metadata.hash_key = blocks.hash_keys[i];
    metadata.size = blocks.block_size[i];
    metadata.replica = 1;
    metadata.type = 0;
    metadata.node = "";
    metadata.l_node = "";
    metadata.r_node = "";
    metadata.is_committed = 1;

    fu.blocks_metadata.push_back(metadata);
  }


  uint32_t file_hash_key = h(name);
  auto socket = connect(file_hash_key);
  send_message(socket.get(), &fu);
  read_reply<Reply>(socket.get());
  socket->close();
}
// }}}
}
