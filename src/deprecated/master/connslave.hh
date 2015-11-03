#ifndef __CONNSLAVE__
#define __CONNSLAVE__

#include <iostream>
#include <common/ecfs.hh>
#include "master_task.hh"

class connslave   // connection to the slave
{
    private:
        int fd;
        int maxmaptask;
        int maxreducetask;
        string address;
        vector<master_task*> running_tasks;
        
    public:
        connslave (int fd);
        connslave (int maxtask, int fd);
        connslave (int maxtask, int fd, string anaddress);
        connslave (string anaddress);
        ~connslave();
        int getfd();
        int getmaxmaptask();
        int getmaxreducetask();
        void setmaxmaptask (int num);
        void setmaxreducetask (int num);
        int getnumrunningtasks();
        master_task* getrunningtask (int index);
        void add_runningtask (master_task* atask);
        void remove_runningtask (master_task* atask);
        void set_address (string anaddress);
        void setfd (int number);
        string get_address();
};

connslave::connslave (int fd)
{
    this->maxmaptask = 0;
    this->maxreducetask = 0;
    this->fd = fd;
}

connslave::connslave (string anaddress)
{
    this->maxmaptask = 0;
    this->maxreducetask = 0;
    this->address = anaddress;
}

connslave::connslave (int maxtask, int fd)
{
    this->maxmaptask = maxtask;
    this->maxreducetask = maxtask;
    this->fd = fd;
}

connslave::connslave (int maxtask, int fd, string anaddress)
{
    this->maxmaptask = maxtask;
    this->maxreducetask = maxtask;
    this->fd = fd;
    this->address = anaddress;
}

connslave::~connslave()
{
    close (fd);
}

int connslave::getfd()
{
    return this->fd;
}

void connslave::setfd (int number)
{
    this->fd = number;
}

int connslave::getmaxmaptask()
{
    return this->maxmaptask;
}

int connslave::getmaxreducetask()
{
    return this->maxreducetask;
}

int connslave::getnumrunningtasks()
{
    return this->running_tasks.size();
}

void connslave::setmaxmaptask (int num)
{
    this->maxmaptask = num;
}

void connslave::setmaxreducetask (int num)
{
    this->maxreducetask = num;
}

master_task* connslave::getrunningtask (int index)
{
    if ( (unsigned) index >= this->running_tasks.size())
    {
        cout << "Index out of bound in the connslave::getrunningtask() function." << endl;
        return NULL;
    }
    else
    {
        return this->running_tasks[index];
    }
}

void connslave::add_runningtask (master_task* atask)
{
    if (atask != NULL)
    {
        running_tasks.push_back (atask);
    }
    else
    {
        cout << "A NULL task is assigned to the running task vector in the connslave::add_runningtask() function." << endl;
    }
}

void connslave::remove_runningtask (master_task* atask)
{
    for (int i = 0; (unsigned) i < running_tasks.size(); i++)
    {
        if (running_tasks[i] == atask)
        {
            running_tasks.erase (running_tasks.begin() + i);
            break;
        }
    }
}

void connslave::set_address (string anaddress)
{
    this->address = anaddress;
}

string connslave::get_address()
{
    return this->address;
}

#endif
