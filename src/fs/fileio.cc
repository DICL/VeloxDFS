#include "fileio.hh"

using namespace std;

namespace eclipse {

  FileIO::FileIO()
  {
  }

  FileIO::~FileIO()
  {
    close_file();
  }

  void FileIO::open_read(int jobid, string name, bool type)
  {
    string path;
    if(type == true) // input or output file
    {
      path = con.settings.get<string>("path.scratch") + "/" + name;
    }
    else // intermediate data
    {
      path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
    }
    auto search = f_map.find(path);
    if(search != f_map.end())
    {
      f_info.jobid = search.jobid;
      f_info.num_block = search.num_block;
      f_info.cur_block = search.cur_block;
      f_info.size = search.size;
      f_info.path = search.path;
      f_info.type = search.type;
      file.open(path.c_str(), ios::in);
    }
    else
    {
      con.logger->error("[fileIO] Job %d: open_r %s failed.", jobid, name.c_str());
    }
  }

  void FileIO::open_write(int jobid, string name, bool type)
  {
    //f_info = new FileInfo;
    f_info.jobid = jobid;
    f_info.num_block = 1;
    f_info.cur_block = 1;
    f_info.type = type;
    f_info.size = 0;
    if(type == true) // input or output file
    {
      f_info.path = con.settings.get<string>("path.scratch") + "/" + name;
      file.open(f_info.path.c_str(), ios::out | ios::app);
    }
    else // intermediate data
    {
      f_info.path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
      string path = f_info.path + "_" + to_string(f_info.num_block);
      file.open(path.c_str(), ios::out | ios::app);
    }
    f_map.insert(pair<string, FileInfo>(f_info.path, f_info));
  }

  void FileIO::read_file(string *buf)
  {
    buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
  }

  void FileIO::read_idata(string *buf)
  {
    getline(file, *buf);
    if(file.eof())
    {
      f_info.cur_block++;
      if(f_info.cur_block < f_info.num_block)
      {
        file.close();
        string path = f_info.path + "_" + to_string(f_info.cur_block + 1);
        file.open(path.c_str(), ios::out | ios::app);
      }
    }
  }

  void FileIO::write_file(const string *buf)
  {
    file << *buf;
  }

  void FileIO::write_idata(const string *buf)
  {
    size_t limit = con.settings.get<int>("block.size");
    if(f_info.size + buf->size() > limit)
    {
      file.close();
      string path = f_info.path + "_" + to_string(++f_info.num_block);
      file.open(path.c_str(), ios::out | ios::app);
    }      
    file << *buf;
  }

  void FileIO::close_file()
  {
    if(file.is_open())
    {
      auto search = f_map.find(path);
      if(search != f_map.end())
      file.close();
  }

  bool FileIO::is_end()
  {
    if(f_info.cur_block < f_info.num_block)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
}
