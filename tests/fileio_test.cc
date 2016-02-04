#include <iostream>
#include "fs/fileio.hh"
#include "common/context.hh"

using namespace eclipse;
using namespace std;

  TEST(fileio) {
	  Context con;
	//Settings setted = Settings().load(); 
	//string logname	= setted.get<string> ("log.name");
	//string logtype	= setted.get<string> ("log.type");
	//Logger* l = Logger::connect(logname	, logtype );
	//Settings setted; 
	FileIO fs1(0), fs2(0), fs3(1), fs4(1), fs5(1);
	/*
	fs.open_writefile("test.txt");
	string msg = "Hello, world!";
	fs.write_line(&msg);
	*/
	string msg1 = "Good!";
	string msg2, msg3, msg4;

	fs1.open_wfile("test.txt");
	fs1.write_file(&msg1);
	fs1.close_file();

	fs2.open_rfile("test.txt");
	fs2.read_file(&msg2);
	con.logger->info("[TEST] msg2 is %s", msg2.c_str());
	fs2.close_file();
	
	fs3.open_iwfile("key1");
	fs3.write_file(&msg2);
	fs3.close_file();

	fs4.open_irfile("key1");
	fs4.read_file(&msg3);
	con.logger->info("[TEST] msg3 is %s", msg3.c_str());
	fs4.close_file();

	fs5.open_rfile("test.txt");
	fs5.read_line(&msg4);
	con.logger->info("[TEST] msg4 is %s", msg4.c_str());
	fs5.close_file();

	//fs.read_line(&msg);
	//fs.read_file(&msg);
	//cout << msg << endl;

	//fs2.open_writefile("test2.txt");
	//fs2.write_line(&msg);
  }
