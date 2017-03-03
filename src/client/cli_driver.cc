#include "cli_driver.hh"
#include "../common/context_singleton.hh"
#include "../common/histogram.hh"
#include <vector>
#include <iostream>
#include <iterator>
#include <iomanip>

using vec_str = std::vector<std::string>;
using namespace std;
using namespace velox;

const string help = "veloxdfs [options] <command> <FILES...>\n\n"
"Commands\n"
"========\n"
"\tput\t\t\tUpload a file\n"
"\tget\t\t\tDownload a file\n"
"\trm\t\t\tRemove a file\n"
"\tcat\t\t\tDisplay a file's content\n"
"\tshow\t\t\tShow block location of a file\n"
"\tls\t\t\tList all the files\n"
"\tformat\t\t\tFormat storage and metadata\n"
"\n"
"Options\n"
"=======\n"
"\t-h, --help\t\tPrint help\n"
"\t-v, --verbose\t\tEnable debugging messages\n"
;


cli_driver::cli_driver() { }

// parse_args {{{
bool cli_driver::parse_args (int argc, char** argv) {
  if (argc < 2) {
    cerr << help << endl;
    return true;
  }

  string command = argv[1];

  if (command == "-h" or command == "--help") {
    cerr << help << endl;
    return true;
  }

  if (command == "ls" or command == "format") {
    if (command == "ls") { 

      bool is_human_readable = false;
      if (argc > 2 && argv[2] == string("-h"))
        is_human_readable = true;

      list(is_human_readable);
    } else format();
   
    return true;
  }

  vec_str files (argv + 2, argv + argc);

  for (auto& file : files) {
    if (command == "put") {
      file_upload(file);

    } else if (command == "get") {
      file_download(file);

    } else if (command == "cat") {
      file_cat(file);

    } else if (command == "show") {
      file_show(file);

    } else if (command == "remove") {
      file_remove(file);

    } else {
      cerr << "[ERR] Unknown operation" << endl;
      cout << help << endl;
    }
  }
  return true;
}
// }}}
// file_upload {{{
void cli_driver::file_upload (std::string file) {
  dfs.upload(file, false);
}
// }}}
// file_download {{{
void cli_driver::file_download (std::string file) {
  dfs.download(file);
}
// }}}
// file_cat {{{
void cli_driver::file_cat (std::string file) {
  // Read file, display it
  cout << dfs.read_all(file) << endl;
}
// }}}
// file_remove {{{
void cli_driver::file_remove (std::string file) {
  dfs.remove(file);
}
// }}}
// file_show {{{
void cli_driver::file_show (std::string file) {
  vec_str nodes = GET_VEC_STR("network.nodes");
  Histogram boundaries(nodes.size(), 100);
  boundaries.initialize();

  model::metadata md = dfs.get_metadata(file);
  cout << file << endl;

  int block_seq = 0;
  for (auto block_name : md.blocks) {
    uint32_t hash_key = md.hash_keys[block_seq++]; 
    int which_node = boundaries.get_index(hash_key);
    int tmp_node;
    for (int i=0; i<(int)md.replica; i++) {
      if (i%2 == 1) {
        tmp_node = (which_node + (i+1)/2 + nodes.size()) % nodes.size();
      } else {
        tmp_node = (which_node - i/2 + nodes.size()) % nodes.size();
      }
      string ip = nodes[tmp_node];
      cout << "\t- " << setw(15) << block_name << " : " << setw(15) << ip << endl;
    }
  }

}
// }}}
// list {{{
void cli_driver::list (bool human_readable) {
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
  vec_str nodes = GET_VEC_STR("network.nodes");
  vector<model::metadata> metadatas = dfs.get_metadata_all(); 

  std::sort(metadatas.begin(), metadatas.end(), [] (const model::metadata& a, const model::metadata& b) {
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

  for (auto& md: metadatas) {
    cout 
      << setw(25) << md.name
      << setw(14) << md.hash_key;
    if (human_readable) {
      float hsize = 0;
      int tabsize = 12;
      string unit;
      cout.precision(2);
      if (md.size < K) {
        hsize = (float)md.size;
        unit = "B";
        tabsize++;
        cout.precision(0);
      } else if (md.size < M) {
        hsize = (float)md.size / KB;
        unit = "KB";
      } else if (md.size < G) {
        hsize = (float)md.size / MB;
        unit = "MB";
      } else if (md.size < T) {
        hsize = (float)md.size / GB;
        unit = "GB";
      } else if (md.size < P) {
        hsize = (float)md.size / TB;
        unit = "TB";
      } else {
        hsize = (float)md.size / PB;
        unit = "PB";
      }
      cout << fixed;
      cout << setw(tabsize) << hsize << unit;
    } else {
      cout << setw(14) << md.size;
    }

    cout
      << setw(8) << md.num_block
      << setw(14) << nodes[md.hash_key % nodes.size()]
      << setw(5) << md.replica
      << endl;
  }
}
// }}}
// format {{{
void cli_driver::format () {
  dfs.format();
}
// }}}

