#pragma once
#include "dfs.hh"

namespace velox {

class vdfs;

class file {
  friend vdfs;
  public:
    void append(std::string);
    std::string get();

  protected:
    file(vdfs*, std::string);
    uint32_t id;
    std::string name;
    vdfs* vdfs_;
};

class vdfs {
  friend file;
  public:
    vdfs();
    ~vdfs();
    velox::file open(std::string);
    velox::file upload(std::string);
    bool rm(std::string);
    bool format();
    //bool exists(std::string);

  protected:
    eclipse::DFS* dfs;
    void append(std::string, std::string);
    std::string load(std::string);

};


}
