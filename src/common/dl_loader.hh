#include "dh_loader.hh"
#include <dlfcn.h>

using namespace std;

// Constructor& destructors {{{
DL_loader::DL_loader(string path, string fun) {
  lib = dlopen(path.c_str(), RTLD_NOW);

  if (!lib) {
    return;
  }

  dlerror();

  func_ = dlsym(lib, fun.c_str); 

  char* err = dlerror();
  if (err) {
    return;
  }
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
