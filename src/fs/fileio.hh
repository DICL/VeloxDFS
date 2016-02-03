#pragma once
#include "../common/context.hh"
#include <iostream>
#include <fstream>
#include <string>

namespace eclipse {
class FileIO {
	private:
		Context con;
		int jobid;
		std::fstream file;

	public:
		FileIO(int);
		~FileIO();

		void open_rfile(std::string);	// open file for read
		void open_wfile(std::string);	// open file for write
		void open_irfile(std::string);	// open intermediate file for read
		void open_iwfile(std::string);	// open intermediate file for write 
		void read_file(std::string *);	// read whole file
		void read_line(std::string *);	// read one line
		void write_file(const std::string *);	// write string
		void close_file();				// close file
};

}
