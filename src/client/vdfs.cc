#include "vdfs.hh"
#include "dfs.hh"
#include "../common/hash.hh"

#include <chrono>
#include <cstring>

using namespace velox;

// Constructors {{{
file::file(vdfs* vdfs_, std::string name_) {
  this->vdfs_ = vdfs_;
  this->name = name_;
  this->opened = false;
  this->id = this->generate_fid();
}

file::file(vdfs* vdfs_, std::string name_, bool opened_) {
  this->vdfs_ = vdfs_;
  this->name = name_;
  this->opened = opened_;
  this->id = this->generate_fid();
}

file::file(const file& that) {
  this->vdfs_ = that.vdfs_;
  this->name = that.name;
  this->opened = that.opened;
  this->id = that.id;
}

// }}}
// generate_fid {{{
long file::generate_fid() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
      ).count();
}
// }}}
// operator= {{{
file& file::operator=(const file& rhs) {
  this->vdfs_ = rhs.vdfs_;
  this->name = rhs.name;
  this->opened = rhs.opened;
  this->id = rhs.id;

  return *this;
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
// open {{{
void file::open() {
  this->opened = true;
}
// }}}
// close {{{
void file::close() {
  this->opened = false;
}
// }}}
// is_open {{{
bool file::is_open() {
  return this->opened;
}
// }}}
// get_id {{{
long file::get_id() {
  return this->id;
}
// }}}
// get_name {{{
std::string file::get_name() {
  return this->name;
}
// }}}
// get_size {{{
long file::get_size() {
  return this->size;
}
// }}}

/******************************************/
/*                                        */
/******************************************/

// vdfs {{{
vdfs::vdfs() {
  dfs = new DFS();
//  dfs->load_settings();

  opened_files = nullptr;
}

vdfs::vdfs(vdfs& that) {
  dfs = new DFS();
  //dfs->load_settings();

  if(that.opened_files != nullptr) 
    opened_files = new std::vector<velox::file>(*that.opened_files);
  else
    opened_files = nullptr;
}

vdfs::~vdfs() {
  if(this->opened_files != nullptr) {
    for(auto f : *(this->opened_files))
      this->close(f.get_id());

    delete this->opened_files;
  }

  delete dfs;
}
// }}}
// operator= {{{
vdfs& vdfs::operator=(vdfs& rhs) {
  if(dfs != nullptr) delete dfs;

  dfs = new DFS();
  //dfs->load_settings();

  if(opened_files != nullptr) delete opened_files;

  if(rhs.opened_files != nullptr)
    opened_files = new std::vector<velox::file>(*rhs.opened_files);
  else
    opened_files = nullptr;

  return *this;
}
// }}}
// open {{{
file vdfs::open(std::string name) {
  // Examine if file is already opened
  if(opened_files != nullptr) {
    for(auto f : *opened_files) {
      if(f.name.compare(name) == 0)
        return f;
    }
  }

  // When a file doesn't exist
  dfs->touch(name);

  velox::file new_file(this, name, true);

  if(opened_files == nullptr) 
    opened_files = new std::vector<velox::file>;

  opened_files->push_back(new_file);

  return new_file;
}
// }}}
// open_file {{{
long vdfs::open_file(std::string fname) {
  return (this->open(fname)).get_id();
}
// }}}
// close {{{
bool vdfs::close(long fid) {
  if(opened_files == nullptr) return false;

  velox::file* f = this->get_file(fid);

  if(f == nullptr) return false;

  f->close();
  return true;
}
// }}}
// is_open() {{{
bool vdfs::is_open(long fid) {
  if(opened_files == nullptr) return false;

  velox::file* f = this->get_file(fid);
  if( f == nullptr) return false;

  return f->is_open();
}
// }}}
// upload {{{
file vdfs::upload(std::string name) {
  dfs->upload(name, false);
  return velox::file(this, name);
}
// }}}
// append {{{
void vdfs::append (std::string name, std::string content) {
  dfs->append(name, content);
}
// }}}
// load {{{
std::string vdfs::load(std::string name) { 
  return dfs->read_all(name);
}
// }}}
// rm {{{
bool vdfs::rm (std::string name) {
  return dfs->remove(name);
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
// write {{{
uint32_t vdfs::write(long fid, const char *buf, uint32_t off, uint32_t len) {
  velox::file* f = this->get_file(fid);
  if(f == nullptr) return -1;

  return dfs->write(f->name, buf, off, len);
}
// }}}
// read {{{
uint32_t vdfs::read(long fid, char *buf, uint32_t off, uint32_t len) {
  velox::file* f = this->get_file(fid);
  if(f == nullptr) return -1;

  return dfs->read(f->name, buf, off, len);
}
// }}}
// get_file {{{
velox::file* vdfs::get_file(long fid) {
  for(auto& f : *(this->opened_files)) {
    if(f.get_id() == fid) 
      return &f;
  }

  return nullptr;
}
// }}}
// get_metadata {{{
model::metadata vdfs::get_metadata(long fid) {
  velox::file* f = this->get_file(fid);
  if(f == nullptr) return model::metadata();

  return dfs->get_metadata(f->name);
}
// }}}

