#include "cli_driver.hh"
#include "../common/context_singleton.hh"
#include <vector>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <unistd.h>
#include <fstream>

using vec_str = std::vector<std::string>;
using namespace std;
using namespace velox;

const string help = R"(
VELOXDFS (VELOX File System CLI client controler)
Usage: veloxdfs [options] <ACTIONS> FILE
ACTIONS
    put <FILE>               Upload a file
    get <FILE>               Download a file
    rm <FILE>                Remove a file
    cat <FILE>               Display a file's content
    show <FILE>              Show block location of a file
    ls -H|-g|-o [FILE]       List all the files
    format                   Format storage and metadata
    rename <FILE1> <FILE2>   Rename file 
  
OPTIONS
    -H           Human,      Show Human readable sizes, MiB|GiB...
    -b <size>    BlockSize,  Set intended size of block for uploading file
    -g           Generate,   Generate logical block distribution
    -o           Output,     Output current logical block distribution
    -h           Help,       Print this text.
    -v           Version,    Print version


Data Intensive Computing Lab at <SKKU/UNIST>, ROK. ver:)" + string(PACKAGE_VERSION) + " Builded at: " + string(__DATE__);


cli_driver::cli_driver() { 
	dfs = new DFS("VeloxClient",-1, true);
}


// parse_args {{{
bool cli_driver::parse_args (int argc, char** argv) {
  try {

    // PARSE OPTIONS
    bool human_readable     = false;
    bool scheduler_generate = false;
    bool scheduler_output   = false;
    uint64_t block_size     = 0;

    int c = 0;
    while ((c = getopt(argc, argv, "hHogb:v")) != -1) {
      switch (c) {
        case 'H': human_readable = true; break;
        case 'h': cout << help << endl; exit(EXIT_SUCCESS); break;
        case 'o': scheduler_output = true; break;
        case 'g': scheduler_generate = true; break;
        case 'b': block_size = atoll(optarg); break;
        case 'v': cout << "VERSION: " << PACKAGE_VERSION << endl; exit(EXIT_SUCCESS); break;
      }
    }

    // PARSE ACTIONS
    if (optind < argc) {
      vector<char*> args (argv, argv + argc);
      string cmd = args.at(optind);
      optind++;

      try {
        if (cmd == "put") {
          file_upload(args.at(optind), block_size);

        } else if (cmd == "get") {
          file_download(args.at(optind));

        } else if (cmd == "rm") {
          file_remove(args.at(optind));

        } else if (cmd == "ls") {

          if (scheduler_output)
            file_show_optimized(args.at(optind), 1);

          else if (scheduler_generate)
            file_show_optimized(args.at(optind), 3);

          else
            list(human_readable);

        } else if (cmd == "cat") {
          file_cat(args.at(optind));

        } else if (cmd == "show") {
          file_show(args.at(optind));

        } else if (cmd == "format") {
          format();

        } else if (cmd == "rename") {
          file_rename(args.at(optind), argv[optind+1]);

        } else if (cmd == "attr") {
          attributes(args.at(optind));

        } else if (cmd == "wt") {
					test_vdfs_write_io(args.at(optind));
				} else if (cmd == "rt") {
					test_vdfs_read_io(args.at(optind));
				} else {
          throw(std::invalid_argument("ERROR: <ACTION> = <" + string(cmd) +">  is not supported"));
        } 
      } catch (std::out_of_range& e) {
        throw(std::invalid_argument("ERROR: <ACTION>'s argument missing or misplaced"));
      }

    } else {
      throw(std::invalid_argument("ERROR: <ACTION> not specified"));
    }

  } catch (std::invalid_argument& e ) {
    cerr << e.what() << endl;
    cerr << help << endl;
  }
  return true;
}
// }}}
// file_upload {{{
void cli_driver::file_upload (std::string file, uint64_t block_size) {
  dfs->upload(file, false, block_size);
}
// }}}
// file_download {{{
void cli_driver::file_download (std::string file) {
  dfs->download(file);
}
// }}}
// file_cat {{{
void cli_driver::file_cat (std::string file) {
  // Read file, display it
  cout << dfs->read_all(file);
}
// }}}
// file_remove {{{
void cli_driver::file_remove (std::string file) {
  dfs->remove(file);
}
// }}}
// file_show {{{
void cli_driver::file_show (std::string file) {
  vec_str nodes = GET_VEC_STR("network.nodes");

  model::metadata md = dfs->get_metadata(file);
  cout << file << endl;

  int block_seq = 0;
  for (auto block_name : md.blocks) {
    uint32_t hash_key = md.hash_keys[block_seq++]; 
    int which_node = GET_INDEX(hash_key);
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
// file_show_optimized {{{
void cli_driver::file_show_optimized(std::string file, int type) {

#ifndef LOGICAL_BLOCKS_FEATURE
  cout << "ERROR! LOGICAL BLOCKS FEATURE IS DISABLED IN THIS BUILD" << endl;
  exit(EXIT_FAILURE);

#else
  vec_str nodes = GET_VEC_STR("network.nodes");

  cout << right 
    << setw(25) << right << "FileName" 
    << setw(14) << right << "FileID"
    << setw(20) << right << "FileSize"
    << setw(45) << right << "BlockName"
    << setw(14) << right << "BlockSize"
    << setw(14) << right << "PyBlocks"
    << setw(14) << right << "Host"
    << endl << string(123,'-') << endl;

  model::metadata md = dfs->get_metadata_optimized(file, type);

  for (unsigned i = 0; i < md.num_block; i++) {
    cout 
      << setw(25) << md.name
      << setw(14) << md.hash_key
      << setw(20) << md.size
      << setw(45) << md.block_data[i].name
      << setw(14) << md.block_data[i].size
      << setw(14) << md.block_data[i].chunks.size()
      << setw(14) << md.block_data[i].host
      << endl;
  } 
#endif
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
  vector<model::metadata> metadatas = dfs->get_metadata_all(); 

  std::sort(metadatas.begin(), metadatas.end(), [] (const model::metadata& a, const model::metadata& b) {
      return (a.name < b.name);
      });
  cout 
    << setw(30) << "FileName" 
    << setw(15) << "Hash Key"
    << setw(20) << "Size"
    << setw(10)  << "Blocks"
    << setw(15) << "Host"
    << setw(5)  << "Repl"
    << endl << string(95,'-') << endl;

  for (auto& md: metadatas) {
    cout 
      << setw(30) << md.name.substr(0, 30)
      << setw(15) << md.hash_key;
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
      cout << setw(20) << (to_string(hsize) + unit);
    } else {
      cout << setw(20) << md.size;
    }

    cout
      << setw(10) << md.num_block
      << setw(15) << nodes[md.hash_key % nodes.size()]
      << setw(5) << md.replica
      << endl;
  }
}
// }}}
// format {{{
void cli_driver::format () {
  dfs->format();
}
// }}}
// rename {{{
void cli_driver::file_rename(std::string src, std::string dst) {
  dfs->rename(src, dst);
}
// }}}
// attributes {{{
void cli_driver::attributes(std::string file) {
  cout << dfs->dump_metadata(file) << endl;
}
// }}}

void cli_driver::test_vdfs_write_io(std::string file ) {
	struct timespec start, end;
	uint64_t len, off = 0;
	uint64_t block_size = context.settings.get<int>("filesystem.block");
	double time = 0;

	std::ifstream ifs (file, ios::in | ios::binary | ios::ate);
	len = ifs.tellg();
	ifs.seekg(0, ios::beg);	
	
	//char* buf = new char[block_size];
	//char* buf = new char[len];
	string upload_str;
	upload_str.reserve(len);
	
//	cout << " File to String Start Len : " << len << endl; 

//	ifs.read(buf, len);
	dfs->touch(file);
	bool t = true;
	uint64_t read_bytes = 0;
	while((long)len > 0) {
	//	bzero(buf, block_size);
		string line;
		while(read_bytes < 1024 * 1024 * 2 && read_bytes < len){
			string line2;
			getline(ifs, line2);
			line2+="\n";
			read_bytes += line2.length();
			line += line2;
		}
		//uint64_t read_bytes = std::min((uint64_t)block_size / 3, len);
		//uint64_t read_bytes = std::min((uint64_t)block_size, len);
		//vector<char> buf(read_bytes);	
		
		//ifs.read(&buf[0], read_bytes);
		clock_gettime(CLOCK_REALTIME, &start);
		uint64_t written_bytes = dfs->write(file, line.c_str(), off, read_bytes, block_size);
		clock_gettime(CLOCK_REALTIME, &end);
		len -= read_bytes;
		off += read_bytes;
		
		time += (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec ) / 1000000000;
		read_bytes = 0;
		cout << "Remaining Len : " << len << endl;
	//	ifs.seekg(written_bytes, ios::beg);
	}
	
	//delete[] buf;
	ifs.close();
	cout <<"Write time: "<< time  << endl;
	
}


void cli_driver::test_vdfs_read_io(std::string file){
	dfs->download(file);
}
