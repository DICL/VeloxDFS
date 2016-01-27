#include "dl_loader.hh"
#include <dlfcn.h>
#include <stdexcept>

using namespace std;

// Constructor& destructors {{{
DL_loader::DL_loader(string path, string fun) {
  lib = dlopen(path.c_str(), RTLD_NOW);

  if (!lib) throw std::runtime_error("Path not found");

  dlerror();

  func_ = reinterpret_cast<void(*)()>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) throw std::runtime_error("Symbol not found");
}

DL_loader::~DL_loader() {
  dlclose(lib);
}
// }}}
// run {{{
void DL_loader::run() {
  this->func_();
}
// }}}
//
