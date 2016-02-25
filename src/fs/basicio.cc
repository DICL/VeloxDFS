#include "basicio.h"
using namespace std;

namespace eclipse {
  BasicIO::BasicIO()  
  {
  }
  BasicIO::~BasicIO() 
  {
    close_file();
  } 
  void BasicIO::open_write(const char* file_name) 
  {
    file.open(file_name, ios::out | ios::app);
  } 
  void BasicIO::open_read(const char* file_name) 
  {
    file.open(file_name, ios::in);
  } 
  void BasicIO::write_file(string *buf) 
  {
    file << *buf << endl;
  }  
  void BasicIO::read_file(string *buf) 
  {
    getline(file, *buf);
  } 
  void BasicIO::close_file() 
  {
    if(file.is_open())
    {
      file.close();
    }
  } 
}
