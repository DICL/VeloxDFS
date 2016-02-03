#include "fileio.hh"

using namespace std;

namespace eclipse {
FileIO::FileIO(Logger* logger, Settings &setted, string file_name, string key, string value, int jobid) {
	this->logger	= logger;
	this->file_name = file_name;
	path			= setted.get<string>("path.scratch") + "/" + file_name;
	ipath			= setted.get<string>("path.scratch") + "/idata/";
	this->key		= key;
	this->value		= value;
	this->jobid		= jobid;
}

bool FileIO::open_readfile()
{
	file.open(path.c_str(), ios::in);
	if(file.is_open())
	{
		logger->info("Can open the file %s for read", file_name.c_str());
		return true;
	}
	logger->error("Can't open the file %s for read", file_name.c_str());
	return false;
}

bool FileIO::open_writefile()
{
	file.open(path.c_str(), ios::out);
	if(file.is_open())
	{
		logger->info("Can open the file %s for write", file_name.c_str());
		return true;
	}
	logger->error("Can't open the file %s for write", file_name.c_str());
	return false;
}

bool FileIO::read_file(string *buf)
{
	if(file.is_open())
	{
		buf->assign((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
		logger->info("Can read the file %s", file_name.c_str());
		return true;
	}
	logger->error("Can't read the file %s", file_name.c_str());
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
		logger->info("Can read line of the file %s", file_name.c_str());
		return true;
	}
	logger->info("Can't read line of the file %s", file_name.c_str());
	return false;
}

bool FileIO::write_file(const string *buf)
{
	if(file.is_open())
	{
		file << *buf;
		logger->info("Can write line to the file %s", file_name.c_str());
		return true;
	}
	logger->info("Can't write line to the file %s", file_name.c_str());
	return false;
}

string FileIO::read_inter()
{
	if(file.is_open())
	{
		getline(file, *buf);
		if(file.eof())
		{
			return false;
		}
		logger->info("Can read line of the file %s", file_name.c_str());
		return true;
	}
	logger->info("Can't read line of the file %s", file_name.c_str());
	return false;

	return "TMP";
}

bool FileIO::write_inter()
{
	return true;
}

string FileIO::display_path()
{
	return path;
}


}
