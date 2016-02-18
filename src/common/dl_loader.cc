#include "dl_loader.hh"
#include <dlfcn.h>
#include <stdexcept>

using namespace std;

// Constructor& destructors {{{
DL_loader::DL_loader(string lib):
 lib_name(lib){ }
DL_loader::~DL_loader() {
}
// }}}
// init_executor {{{
bool DL_loader::init_lib () {
  lib = dlopen(lib_name.c_str(), RTLD_NOW);

  if (!lib) throw std::runtime_error("Path not found");

  dlerror();
  return true;
}
// }}}
// load_function {{{
maptype DL_loader::load_function (std::string fun) {
  maptype func_ = 
    reinterpret_cast<maptype>(dlsym(lib, fun.c_str())); 
  char* err = dlerror();

  if (err) throw std::runtime_error("Symbol not found");
  return func_;
}
// }}}
void DL_loader::close() {
  dlclose(lib);
}
