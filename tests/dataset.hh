#include <mapreduce/dataset.hh>
#include <mapreduce/input.hh>



int main () {
  Dataset a = Input::open("sdsds");
  Dataset b = a.map("function");
  return 0;
}
