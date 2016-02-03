#include "dl_loader.hh"
#include <dlfcn.h>
#include <stdexcept>

using namespace std;

// Constructor& destructors {{{
DL_loader::DL_loader(string lib, string in):
  input(in), lib_name(lib){ }
DL_loader::~DL_loader() {
  dlclose(lib);
}
// }}}
// init_executor {{{
bool DL_loader::init_executor (PeerLocalMR*) {
  lib = dlopen(path.c_str(), RTLD_NOW);

  if (!lib) throw std::runtime_error("Path not found");

  dlerror();
}
// }}}
// load_function {{{
maptype DL_loader::load_function (std::string) {
  maptype func_ = 
    reinterpret_cast<maptype>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) throw std::runtime_error("Symbol not found");
  return func_;
}
// }}}
