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
      auto search = f_map.find(path);
      if(search != f_map.end())
      {
        *f_info = search->second;
        file.open(path.c_str(), ios::in);
      }
      else
      {
        con.logger->error("[fileIO] Job %d: open_r %s failed.", jobid, name.c_str());
      }
    }
    else // intermediate data
    {
      path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
      auto search = f_map.find(path);
      if(search != f_map.end())
      {
        *f_info = search->second;
        path = path + "_" + to_string(f_info->cur_block);
        file.open(path.c_str(), ios::in);
      }
      else
      {
        con.logger->error("[fileIO] Job %d: open_r %s failed.", jobid, name.c_str());
      }
    }
  }

  void FileIO::open_write(int jobid, string name, bool type)
  { 
    FileInfo tmp_info;
    tmp_info.jobid = jobid;
    tmp_info.num_block = 1;
    tmp_info.cur_block = 1;
    tmp_info.type = type;
    tmp_info.size = 0;
    if(type == true) // input or output file
    {
      tmp_info.path = con.settings.get<string>("path.scratch") + "/" + name;
      file.open(tmp_info.path.c_str(), ios::out | ios::app);
    }
    else // intermediate data
    {
      tmp_info.path = con.settings.get<string>("path.idata") + "/" + to_string(jobid) + "_" + name;
      string path = tmp_info.path + "_" + to_string(tmp_info.num_block);
      file.open(path.c_str(), ios::out | ios::app);
    }
    f_map.insert(pair<string, FileInfo>(tmp_info.path, tmp_info));
    auto search = f_map.find(tmp_info.path);
    f_info = &(search->second);
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
      f_info->cur_block++;
      if(f_info->cur_block < f_info->num_block)
      {
        file.close();
        string path = f_info->path + "_" + to_string(f_info->cur_block + 1);
        file.open(path.c_str(), ios::in | ios::app);
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
     // con.logger->info("size of size = %d", f_info->size);
    //string tmp = *buf;
      //con.logger->info("size of tmp = %d", tmp.length());
      //con.logger->info("tmp = %s", tmp.c_str());

    
    if(f_info->size + buf->length() > limit)
    {
      //con.logger->info("OVER!!!!");
      file.close();
      string path = f_info->path + "_" + to_string(++(f_info->num_block));
      file.open(path.c_str(), ios::out | ios::app);
      f_info->size = 0;
    }      
    file << *buf;
    f_info->size += buf->length();
      //con.logger->info("size of tmp = %d", tmp.length());
      //con.logger->info("size of size = %d", f_info->size);
  }

  void FileIO::close_file()
  {
    if(file.is_open())
    {
      file.close();
    }
  }

  bool FileIO::is_end()
  {
    if(f_info->cur_block < f_info->num_block)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
}
