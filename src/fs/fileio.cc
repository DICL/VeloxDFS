#include "fileio.hh"

using namespace std;

namespace eclipse {
void FileIO::open_rfile(string file_name)
{
	string path = con.settings.get<string>("path.scratch") + "/" + file_name;
	file.open(path.c_str(), ios::in);
}

void FileIO::open_wfile(string file_name)
{
	string path = con.settings.get<string>("path.scratch") + "/" + file_name;
	file.open(path.c_str(), ios::out);
}

void FileIO::open_irfile(string key)
{
	string path = con.settings.get<string>("path.idata") + to_string(jobid) + "_" + key;
	file.open(path.c_str(), ios::in);
}

void FileIO::open_iwfile(string key)
{
	string path = con.settings.get<string>("path.idata") + to_string(jobid) + "_" + key;
	file.open(path.c_str(), ios::out);
}

void FileIO::read_file(string *buf)
{
	buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
}

void FileIO::read_line(string *buf)
{
	getline(file, *buf);
}

void FileIO::write_file(const string *buf)
{
	file << *buf;
}

}
