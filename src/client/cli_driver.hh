#pragma once
#include "dfs.hh"

class cli_driver {
  public:
    cli_driver();
    ~cli_driver() = default;

    bool parse_args(int argc, char** argv);

  private:
    void file_upload(std::string, uint64_t block_size);
    void file_download(std::string);
    void file_cat(std::string);
    void file_remove(std::string);
    void file_show(std::string);

    void list(bool human_readable);
    void file_show_optimized(std::string path, int type);
    void format();
    void file_rename(std::string, std::string);
    void attributes(std::string);
	
		void test_vdfs_write_io(std::string);
		void test_vdfs_read_io(std::string);
    velox::DFS *dfs;
    //velox::DFS dfs(-1,0);
};
