// includes & usings {{{
#include "file_leader.hh"
#include "../messages/boost_impl.hh"
#include "../messages/filedescription.hh"
#include "../common/logical_block_metadata.hh"

#ifdef LOGICAL_BLOCKS_FEATURE
#include "../stats/logical_blocks_scheduler.hh"
#endif

#include <set>

using namespace eclipse;
using namespace eclipse::messages;
using namespace eclipse::network;
using namespace std;

// }}}

// Constructor & destructor {{{
FileLeader::FileLeader (ClientHandler* net) : Node () { 
  network = net;

  network_size = context.settings.get<vec_str>("network.nodes").size();
  boundaries.reset( new Histogram {network_size, 100});
  boundaries->initialize();

  directory.create_tables();
}

FileLeader::~FileLeader() { }
// }}}
// file_insert {{{
//! @attention The block metadata is a proposal, the client
//! might endup with more blocks.
//! @todo fix block shortage in client
//! @todo strategy pattern for scheduling the blocks
unique_ptr<Message> FileLeader::file_insert(messages::FileInfo* f) {
  directory.file_table_insert(*f);
  INFO("Saving file: %s to SQLite db", f->name.c_str());

  // ip = schedule_block(index, size);
  FileDescription* fd = new FileDescription();

  uint32_t size_per_block = GET_INT("filesystem.block");
  uint32_t n_blocks = static_cast<uint32_t> (ceil((double)f->size /(double) size_per_block));
  INFO("%u block to be save for file %s", n_blocks, f->name.c_str());
  fd->name = f->name;
  fd->size = f->size;
  fd->hash_key = f->hash_key;

  return unique_ptr<Message>(fd);
}
// }}}
// file_insert_confirm {{{
bool FileLeader::file_insert_confirm(messages::FileInfo* f) {
  directory.file_table_confirm_upload(f->name, f->num_block);

  for (auto& metadata : f->blocks_metadata) {
    directory.block_table_insert(metadata);
  }

  replicate_metadata();

  return true;
}
// }}}
// file_update {{{
bool FileLeader::file_update(messages::FileUpdate* f) {
  if (file_exist(f->name)) {
    DEBUG("[file_update] name: %s, size: %lu, num_block: %d", f->name.c_str(), f->size, f->num_block);

    if (f->is_append) {
      BlockInfo bi;
      directory.select_last_block_metadata(f->name, &bi);
      int last_seq = bi.seq;

      for (auto& metadata : f->blocks_metadata) {
        metadata.seq = ++last_seq;
        directory.block_table_insert(metadata);
      }

      FileInfo fi;
      directory.file_table_select(f->name, &fi);
      directory.file_table_update(f->name, f->size + fi.size, last_seq + 1);

    } else {
      directory.file_table_update(f->name, f->size, f->num_block);
      for (auto& metadata : f->blocks_metadata) {
        directory.block_table_insert(metadata);
      }
    }

    INFO("Updating to SQLite db");
    return true;
  }

  return false;
}
// }}}
// file_delete {{{
bool FileLeader::file_delete(messages::FileDel* f) {
  if (file_exist(f->name)) {
    directory.file_table_delete(f->name);
    directory.block_table_delete_all(f->name);
    replicate_metadata();
    INFO("Removing from SQLite db");
    return true;
  }
  return false;
}
// }}}
// file_request {{{
unique_ptr<Message> FileLeader::file_request(messages::FileRequest* m) {
  string file_name = m->name;

  FileInfo fi;
  fi.num_block = 0;
  FileDescription* fd = new FileDescription();
  fd->name = file_name;

  directory.file_table_select(file_name, &fi);
  fd->uploading = fi.uploading;

  if (fi.uploading == 1) //! Cancel if file is being uploading
    return unique_ptr<Message>(fd);

  fd->hash_key = fi.hash_key;
  fd->replica = fi.replica;
  fd->size = fi.size;
  fd->num_block = fi.num_block;

  int num_blocks = fi.num_block;
  for (int i = 0; i < num_blocks; i++) {
    BlockInfo bi;
    directory.block_table_select(file_name, i, &bi);
    string block_name = bi.name;
    fd->blocks.push_back(block_name);
    fd->hash_keys.push_back(bi.hash_key);
    fd->block_size.push_back(bi.size);
    fd->block_hosts.push_back(bi.node);
  }

  if (m->type == "LOGICAL_BLOCKS")
    find_best_arrangement(fd);

  return unique_ptr<Message>(fd);
}
// }}}
// list {{{
bool FileLeader::list (messages::FileList* m) {
  directory.file_table_select_all(m->data);
  return true;
}
// }}}
// file_exist {{{
bool FileLeader::file_exist (std::string file_name) {
  return directory.file_table_exists(file_name);
}
// }}}
// replicate_metadata {{{
//! @brief This function replicates to its right and left neighbor
//! node the metadata db. 
//! This function is intended to be invoked whenever the metadata db is modified.
void FileLeader::replicate_metadata() {
  MetaData md; 
  md.node = context.settings.getip();
  md.content = local_io.read_metadata();

  int left_node = ((id - 1) < 0) ? network_size - 1: id - 1;
  int right_node = ((id + 1) == network_size) ? 0 : id + 1;

  network->send(left_node, &md);
  network->send(right_node, &md);
}
// }}}
// metadata_save {{{
void FileLeader::metadata_save(MetaData* m) {
  std::string file_name = m->node + "_replica";
  local_io.write(file_name, m->content);
}
// }}}
// format {{{
bool FileLeader::format () {
  INFO("Formating DFS");
  local_io.format();
  directory.create_tables();
  return true;
}
// }}}
// find_best_arrangement {{{
void FileLeader::find_best_arrangement(messages::FileDescription* file_desc) {
#ifdef LOGICAL_BLOCKS_FEATURE
  using namespace eclipse::logical_blocks_schedulers;
  auto nodes = context.settings.get<vec_str>("network.nodes");

  map<string, string> opts;
  opts["alpha"] = GET_STR("addons.alpha");
  opts["beta"]  = GET_STR("addons.beta");

  auto scheduler = scheduler_factory(GET_STR("addons.block_scheduler"), boundaries.get(), opts);

  scheduler->generate(*file_desc, nodes);
#endif
}
// }}}
