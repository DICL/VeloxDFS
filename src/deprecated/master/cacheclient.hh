#ifndef __CACHECLIENT__
#define __CACHECLIENT__

#include <iostream>

using namespace std;

// address of master should be same as cache server
class master_connection
{
    private:
        int fd;
        
    public:
        master_connection();
        master_connection (int num);
        int get_fd();
        void set_fd (int num);
};

master_connection::master_connection()
{
    fd = -1;
}

master_connection::master_connection (int num)
{
    fd = num;
}

int master_connection::get_fd()
{
    return fd;
}

void master_connection::set_fd (int num)
{
    fd = num;
}


class cacheclient
{
    private:
        int fd;
        string address;
        
    public:
        cacheclient (int number, string anaddress);
        cacheclient (string anaddress);
        int get_fd();
        void set_fd (int num);
        string get_address();
        void set_address (string anaddress);
};

cacheclient::cacheclient (int number, string anaddress)
{
    fd = number;
    address = anaddress;
}

cacheclient::cacheclient (string anaddress)
{
    fd = -1;
    address = anaddress;
}

int cacheclient::get_fd()
{
    return fd;
}

void cacheclient::set_fd (int num)
{
    fd = num;
}

string cacheclient::get_address()
{
    return address;
}

void cacheclient::set_address (string anaddress)
{
    address = anaddress;
}

#endif
