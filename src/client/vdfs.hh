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
    vdfs(std::string job_id, int tid, bool initializer);
    vdfs(vdfs&);
    ~vdfs();

    vdfs& operator=(vdfs&);

    velox::file open(std::string);

    long open_file(std::string);
    bool close(long);
    bool is_open(long);

    velox::file upload(std::string);
	velox::file upload_idv(std::string);
    bool rm(std::string);
    bool rm(long);
    bool format();

    bool exists(std::string);

    uint32_t write(long, const char*, uint32_t, uint32_t);
    uint32_t write(long, const char*, uint32_t, uint32_t, uint64_t);
    uint32_t read(long, char*, uint64_t, uint64_t);
    //uint32_t read_chunk(std::string name, std::string host, char *buf, uint32_t boff, uint64_t , uint64_t, int);
    uint32_t read_chunk(char *buf, uint32_t boff);
    //int* read_chunk(std::string name, std::string host, char *buf, uint32_t boff, uint64_t , uint64_t, int);

    model::metadata get_metadata(long fid, int type);

    void append(std::string, std::string);

    std::vector<model::metadata> list(bool all, std::string name = "");

    bool rename(std::string, std::string);

		velox::file write_file(std::string, const std::string&, uint64_t);
	
	int get_tmg_id();
	
	int tmg_id;
	std::string job_id;
	bool initializer;
  protected:
    DFS* dfs;
    std::string load(std::string);

  private:
    velox::file* get_file(long);
    std::vector<velox::file>* opened_files;
};


}
