#include <iostream>
#include "basicio.h"
using namespace std;
using namespace eclipse;

int main()
{
  BasicIO bio;
  string str = "Hello, World!";
  string ostr;
  bio.open_write("output.txt");
  bio.write_file(&str);
  bio.close_file();
  bio.open_read("output.txt");
  bio.read_file(&ostr);
  cout << ostr << endl;;
  bio.close_file();
  return 0;
}
