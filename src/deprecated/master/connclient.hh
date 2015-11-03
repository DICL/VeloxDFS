#ifndef __CONNCLIENT__
#define __CONNCLIENT__

#include <iostream>

class connclient   // connection to the client
{
    private:
        int fd;
        
    public:
        connclient (int fd);
        ~connclient();
        int getfd();
        void setfd (int num);
        
};

connclient::connclient (int fd)
{
    this->fd = fd;
}

connclient::~connclient()
{
    close (fd);
}

int connclient::getfd()
{
    return this->fd;
}

void connclient::setfd (int num)
{
    this->fd = num;
}

#endif
