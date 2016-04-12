#include <iostream>
#include "messages/fileinfo.hh"
#include "messages/blockinfo.hh"
#include "fs/directory.hh"
#include "common/context.hh"

using namespace eclipse;
using namespace std;

int main() {
  Directory dir;
  context.logger->info("==========Start dir test==========");

  // Basic metadata io example
  dir.init_db();
  FileInfo file_info;
  BlockInfo block_info;

  file_info.file_name = "test.txt";
  file_info.file_hash_key = 1;
  file_info.file_size = 1;
  file_info.num_block = 1;
  file_info.replica = 3;

  block_info.file_name = "text.txt";
  block_info.block_seq = 1;
  block_info.block_hash_key = 1;
  block_info.block_name = "test.txt_1";
  block_info.block_size = 1;
  block_info.is_inter = 1;
  block_info.node = "1.1.1.1";
  block_info.l_node = "1.1.1.0";
  block_info.r_node = "1.1.1.2";
  block_info.is_committed = 3;

  dir.insert_file_metadata(file_info);
  dir.insert_block_metadata(block_info);

  file_info.file_name = "test2.txt";
  dir.insert_file_metadata(file_info);

  file_info.file_name = "test3.txt";
  dir.insert_file_metadata(file_info);

  //dir.display_file_metadata();
  //dir.display_block_metadata();
  //dir.delete_file_metadata("test.txt");
  //dir.delete_block_metadata("text.txt", 1);
  //dir.display_file_metadata();
  //dir.display_block_metadata();

  /*
  FileInfo file_info;
  BlockInfo block_info;

  file_info.file_id = 1;
  file_info.file_name = "test.txt";
  file_info.file_hash_key = 1;
  file_info.file_size = 1;
  file_info.num_block = 1;
  file_info.replica = 1;

  block_info.file_id = 1;
  block_info.block_seq = 1;
  block_info.block_hash_key = 1;
  block_info.block_name = "test.txt_1";
  block_info.block_size = 1;
  block_info.is_inter = 1;
  block_info.node = "1.1.1.1";
  block_info.l_node = "1.1.1.0";
  block_info.r_node = "1.1.1.2";
  block_info.is_commit = 1;

  dir.delete_file_metadata(1);
  dir.delete_block_metadata(1, 1);
  dir.display_file_metadata();
  dir.display_block_metadata();

  if(!dir.is_exist(file_info.file_id))
  {
    con.logger->info("Not exist");
    dir.insert_file_metadata(file_info);
  }
  else
  {
    con.logger->info("Already exist");
  }
  */
  /*
  FileInfo file_info2;
  dir.delete_file_metadata(2);
  dir.select_file_metadata(1, &file_info2);
  file_info2.file_id = 2;
  dir.update_file_metadata(1, file_info2);
  dir.display_file_metadata();
  dir.display_block_metadata();
  */
  context.logger->info("==========End dir test==========");
  return 0;
}
