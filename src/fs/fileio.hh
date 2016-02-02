#pragma once
#include "../common/logger.hh"
#include "../common/settings.hh"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

namespace eclipse {
class FileIO {
	private:
		Logger* logger;
		fstream file;
		string file_name, path, key, value;
		int jobid;

	public:
		FileIO(Logger*, Settings &, string, string, string, int);
		~FileIO(){file.close(); }

		bool open_readfile();
		bool open_writefile();
		bool read_file(string *);
		bool read_line(string *);
		bool write_file(const string *);
		string read_inter();
		bool write_inter();

		// for test
		string display_path();
};

}

