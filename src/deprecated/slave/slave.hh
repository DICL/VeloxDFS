#ifndef _SLAVE_
#define _SLAVE_

#define DHT_SLAVE

#include "slave_task.hh"

void connect_to_server (const char *host, unsigned short port);   // function which connect to the master
void signal_listener (void);   // function used to communicate with the master
void launch_task (slave_task* atask);   // launch forwarded task
slave_job* find_jobfromid (int id);   // return the slave_job with input id if it exist

#endif
