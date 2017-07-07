#pragma once
#include "dfs.hh"

class cli_driver {
  public:
    cli_driver();
    ~cli_driver() = default;

    bool parse_args(int argc, char** argv);

  private:
    void file_upload(std::string);
    void file_download(std::string);
    void file_cat(std::string);
    void file_remove(std::string);
    void file_show(std::string);

    void list(bool human_readable);
    void file_show_optimized(std::string path);
    void format();

    velox::DFS dfs;
};
