// Headers {{{
#include "dfs.hh"
#include "../messages/boost_impl.hh"
#include "../messages/fileinfo.hh"
#include "../messages/factory.hh"
#include "../messages/fileinfo.hh"
#include "../messages/blockinfo.hh"
#include "../messages/blockupdate.hh"
#include "../messages/fileexist.hh"
#include "../messages/filedescription.hh"
#include "../messages/filerequest.hh"
#include "../messages/filelist.hh"
#include "../messages/blockdel.hh"
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
  fr.only_metadata = only_metadata;

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
  return ifile;
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
  vector<char> chunk(BLOCK_SIZE);
  Histogram boundaries(NUM_NODES, 100);
  boundaries.initialize();

  FILETYPE type = FILETYPE::Normal;

  //! If it is an app to be upload :TODO:
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
  int i = 0;

  vector<BlockMetadata> blocks_metadata;
  vector<future<bool>> slave_sockets;

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
    int which_server = description->hash_keys[i] % NUM_NODES;
    block.first = metadata.name = description->blocks[i];
    metadata.file_name = file_name;
    metadata.hash_key = description->hash_keys[i];
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
    io_ops.operation = "BLOCK_INSERT";
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
    io_ops.operation = "BLOCK_REQUEST";
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
    io_ops.operation = "BLOCK_REQUEST";
    io_ops.block.first = fd->blocks[index++];

    auto slave_socket = connect(boundaries.get_index(fd->hash_keys[index]));
    send_message(slave_socket.get(), &io_ops);
    auto msg = read_reply<IOoperation>(slave_socket.get());
    output += msg->block.second;
    slave_socket->close();
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
    io_ops.operation = "BLOCK_DELETE";
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

  while (to_write_byte > 0) { // repeat until to_write_byte == 0
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
  block_info.content = "";

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
// write {{{
uint32_t DFS::write(std::string& file_name, const char* buf, uint64_t off, uint64_t len) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  auto fd = get_file_description(
    std::bind(&connect, std::placeholders::_1), file_name
  );
  if(fd == nullptr) return 0;

  off = std::max(0ul, std::min(off, std::max(fd->size, BLOCK_SIZE - 1)));

  uint64_t to_write_bytes = len;
  uint64_t written_bytes = 0;

  int block_beg_seq = (int) off / BLOCK_SIZE;
  int block_end_seq = (int) (len + off - 1) / BLOCK_SIZE;

  for(int i=block_beg_seq; i<=block_end_seq; i++) {
    if(i < (int)fd->num_block) { // updating exist block
      uint32_t hash_key = fd->hash_keys[i];

      // send message
      BlockUpdate bu;
      bu.name = fd->blocks[i]; 
      bu.file_name = file_name;
      bu.seq = i;
      bu.replica = fd->replica; 
      bu.hash_key = hash_key; 
      bu.pos = (i == block_beg_seq && fd->block_size[i] > 0) ? (off % BLOCK_SIZE) : 0;
      bu.len = (fd->block_size[i] == 0) ? std::min(to_write_bytes, BLOCK_SIZE) : std::min((BLOCK_SIZE - bu.pos), to_write_bytes);
      string content_str(buf + written_bytes, bu.len);
      bu.content = content_str;
      bu.size = std::max(fd->block_size[i], bu.len);

      auto block_socket = connect(boundaries.get_index(bu.hash_key));
      send_message(block_socket.get(), &bu);
      auto reply = read_reply<Reply> (block_socket.get());
      block_socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return 0;
      } 

      written_bytes += bu.len;
    }
    else { // creating a new block
      int which_server = ((fd->hash_key % NUM_NODES) + i) % NUM_NODES;

      BlockInfo bi;
      bi.name = file_name + "_" + to_string(i);
      bi.file_name = file_name;
      bi.hash_key = boundaries.random_within_boundaries(which_server);
      bi.seq = i;
      bi.size = std::min(BLOCK_SIZE, to_write_bytes);
      bi.type = static_cast<unsigned int>(FILETYPE::Normal);
      bi.replica = fd->replica;
      bi.node = nodes[which_server];
      bi.l_node = nodes[(which_server - 1 + NUM_NODES) % NUM_NODES];
      bi.r_node = nodes[(which_server + 1 + NUM_NODES) % NUM_NODES];
      bi.is_committed = 1;
      string content_str(buf + written_bytes, bi.size);
      bi.content = content_str;

      auto block_socket = connect(fd->hash_key);
      send_message(block_socket.get(), &bi);
      auto reply = read_reply<Reply> (block_socket.get());
      block_socket->close();

      if (reply->message != "OK") {
        cerr << "[ERR] Failed to upload file. Details: " << reply->details << endl;
        return 0;
      } 

      written_bytes += bi.size;
    }

    to_write_bytes -= written_bytes;
  }

  // Update Metadata
  FileUpdate fu;
  fu.name = fd->name;
  fu.num_block = std::max(fd->num_block, (unsigned int)block_end_seq + 1);
  fu.size = std::max(written_bytes + off, fd->size);
  auto socket = connect(fd->hash_key);
  send_message(socket.get(), &fu);
  auto reply = read_reply<Reply> (socket.get());
  socket->close();

  return written_bytes;
}
// }}}
// read {{{
uint32_t DFS::read(std::string& file_name, char* buf, uint64_t off, uint64_t len) {
  Histogram boundaries(NUM_NODES, 0);
  boundaries.initialize();

  auto fd = get_file_description(
    std::bind(&connect, std::placeholders::_1), file_name
  );
  if(fd == nullptr) return 0;

  off = std::max(0ul, std::min(off, fd->size));
  if(off >= fd->size) return 0;

/*
  uint32_t file_hash_key = h(file_name);
  auto socket = connect(file_hash_key);

  FileRequest fr;
  fr.name = file_name;

  send_message(socket.get(), &fr);
  auto fd = read_reply<FileDescription> (socket.get());

  socket->close();
  */

  int block_beg_seq = (int) off / BLOCK_SIZE;
  int block_end_seq = (int) (len + off - 1) / BLOCK_SIZE;

  std::string output = "";

  uint32_t remain_len = len;

  for(int i=block_beg_seq; i<=block_end_seq; i++) {
    uint32_t hash_key = fd->hash_keys[i];
    auto block_socket = connect(boundaries.get_index(hash_key));

    BlockRequest br;
    br.name = fd->blocks[i]; 
    br.hash_key = hash_key; 
    br.off = (i == block_beg_seq && fd->block_size[i] > 0) ? (off % fd->block_size[i]) : 0;
    br.len = std::min((fd->block_size[i] - br.off), remain_len);
    br.should_read_partially = true;

    send_message(block_socket.get(), &br);
    auto msg = read_reply<BlockInfo>(block_socket.get());
    output += msg->content;
    
    block_socket->close();

    remain_len -= br.len;

    if(br.off + br.len > fd->block_size[i])
      break;
  }

  strcpy(buf, output.c_str());

  return (uint32_t)output.length();
}
// }}}
// get_metadata {{{
model::metadata DFS::get_metadata(std::string& fname) {
  model::metadata md;

  auto fd = get_file_description(
    std::bind(&connect, std::placeholders::_1), fname, true
  );
  if(fd != nullptr) {
    md.name = fd->name;
    md.hash_key = fd->hash_key;
    md.size = fd->size;
    md.num_block = fd->num_block;
    md.type = fd->type;
    md.replica = fd->replica;
    md.blocks = fd->blocks;
    md.hash_keys = fd->hash_keys;
    md.block_size = fd->block_size;
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
}
