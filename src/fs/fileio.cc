#include "fileio.hh"

using namespace std;

namespace eclipse {
FileIO::FileIO(Logger* l, Settings &setted, string file_name, string key, string value, int jobid) {
	this->logger = l;
	this->file_name = file_name;
	path			= setted.get<string> ("path.scratch") + "/" + file_name;
	this->key		= key;
	this->value		= value;
	this->jobid		= jobid;
}

bool FileIO::open_readfile()
{
	file.open(path.c_str(), ios::in);
	if(file.is_open())
		return true;
	logger->error("Can't open the file %s", file_name.c_str());
	return false;
}

bool FileIO::open_writefile()
{
	file.open(path.c_str(), ios::out);
	if(file.is_open())
		return true;
	logger->error("Can't open the file %s", file_name.c_str());
	return false;
}

bool FileIO::read_file(string *buf)
{
	if(file.is_open())
	{
		buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
		return true;
	}
	logger->error("Can't open the file");
	return false;
}

bool FileIO::read_line(string *buf)
{
	if(file.is_open())
	{
		getline(file, *buf);
		if(file.eof())
		{
			return false;
		}
		return true;
	}
	logger->error("Can't open the file");
	return false;
}

bool FileIO::write_file(const string *buf)
{
	if(file.is_open())
	{
		file << *buf;
		return true;
	}
	logger->error("Can't open the file");
	return false;
}

string FileIO::read_inter()
{
	return "TMP";
}

bool FileIO::write_inter()
{
	return true;
}

}
