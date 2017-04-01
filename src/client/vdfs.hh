#pragma once
#include "dfs.hh"
#include <vector>

namespace velox {

class vdfs;

class file {
  friend vdfs;
  public:
    void append(std::string);
    std::string get();

    void open();
    void close();
    bool is_open();

    long get_id();
    std::string get_name();
    long get_size();

    file& operator=(const file&);

    file(vdfs*, std::string);
    file(vdfs*, std::string, bool);
    file(const file&);

  private:
    long id;
    std::string name;
    vdfs* vdfs_;
    bool opened;

    long size;

    long generate_fid();
};

class vdfs {
  friend file;
  public:
    vdfs();
    vdfs(vdfs&);
    ~vdfs();

    vdfs& operator=(vdfs&);

    velox::file open(std::string);

    long open_file(std::string);
    bool close(long);
    bool is_open(long);

    velox::file upload(std::string);

    bool rm(std::string);
    bool format();

    bool exists(std::string);

    uint32_t write(long, const char*, uint32_t, uint32_t);
    uint32_t read(long, char*, uint32_t, uint32_t);

    model::metadata get_metadata(long fid);

    void append(std::string, std::string);

  protected:
    DFS* dfs;
    std::string load(std::string);

  private:
    velox::file* get_file(long);
    std::vector<velox::file>* opened_files;
};


}
