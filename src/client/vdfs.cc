#include "vdfs.hh"
#include "dfs.hh"
#include "../common/hash.hh"

using namespace velox;

// Constructors {{{
file::file(vdfs* vdfs_, std::string name_) {
  this->vdfs_ = vdfs_;
  name = name_;
}

// }}}
// push_back {{{
void file::append(std::string content) {
  vdfs_->append(name, content);
}
// }}}
// get {{{
std::string file::get() {
  return vdfs_->load(name);
}
// }}}
// vdfs {{{
vdfs::vdfs() {
  dfs = new DFS();
  dfs->load_settings();
}

vdfs::~vdfs() {
  delete dfs;
}
// }}}
// open {{{
file vdfs::open(std::string name) {
  if (!dfs->exists(name)) 
    dfs->touch(name);

  return velox::file(this, name);
}
// }}}
// upload {{{
file vdfs::upload(std::string name) {
  dfs->put({"","", name});
  return velox::file(this, name);
}
// }}}
// append {{{
void vdfs::append (std::string name, std::string content) {
  dfs->push_back({"", "", name, content});
}
// }}}
// load {{{
std::string vdfs::load(std::string name) { 
  return dfs->load({name});
}
// }}}
// rm {{{
bool vdfs::rm (std::string name) {
  return dfs->rm({"", "", name});
}
// }}}
// format {{{
bool vdfs::format () {
  return dfs->format();
}
// }}}
// exists {{{
bool vdfs::exists(std::string name) {
  return dfs->exists(name);
}
// }}}
