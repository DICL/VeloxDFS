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
		FileIO(int jid) : jobid(jid){}
		~FileIO(){close_file();}

		void open_rfile(std::string);
		void open_wfile(std::string);
		void open_irfile(std::string);
		void open_iwfile(std::string);
		void read_file(std::string *);
		void read_line(std::string *);
		void write_file(const std::string *);
		void close_file(){if(file.is_open()) file.close();}
};

}
