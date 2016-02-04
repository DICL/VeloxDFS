#include <iostream>
#include "fs/fileio.hh"
#include "common/context.hh"

using namespace eclipse;
using namespace std;

  TEST(fileio) {
    Context con;
	FileIO fs1;
	string msg1 = "Good!";
  string msg3 = "Great!";
	string msg2, msg4;

	fs1.open_write(0, "test.txt", true);
	fs1.write_file(&msg1);
	fs1.close_file();

	fs1.open_read(1, "test.txt", true);
	fs1.read_file(&msg2);
	con.logger->info("[TEST] msg2 is %s", msg2.c_str());
	fs1.close_file();

	fs1.open_write(2, "key1", false);
	fs1.write_idata(&msg3);
	fs1.close_file();

	fs1.open_read(2, "key1", false);
	fs1.read_idata(&msg4);
	con.logger->info("[TEST] msg4 is %s", msg4.c_str());
	fs1.close_file();

  }
