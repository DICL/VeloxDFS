#include <mapreduce/dataset.hh>
using namespace eclipse;

int main () {
  DataSet& a = DataSet::open("sdsds");
  DataSet& b = a.map("function");
  return 0;
}
