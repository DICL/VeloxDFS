#include <iostream>
#include "fs/fileio.hh"
#include "common/logger.hh"

using namespace eclipse;
using namespace std;

  TEST(fileio) {
	Settings setted = Settings().load(); 
	string logname	= setted.get<string> ("log.name");
	string logtype	= setted.get<string> ("log.type");
	Logger* l = Logger::connect(logname	, logtype );
	//Settings setted; 
	FileIO fs(l, setted, "test2.txt", "unist", "berkeley", 1);
	/*
	fs.open_writefile("test.txt");
	string msg = "Hello, world!";
	fs.write_line(&msg);
	*/
	fs.open_writefile();
	string msg = "Hello, world!";
	fs.write_file(&msg);
	string path = fs.display_path();
	l->info("path is %s", path.c_str()); 
	//fs.read_line(&msg);
	//fs.read_file(&msg);
	//cout << msg << endl;

	//fs2.open_writefile("test2.txt");
	//fs2.write_line(&msg);
	Logger::disconnect(l);
  }
