#ifndef __MASTER__
#define __MASTER__

#include <iostream>
#include <string>
#include "master_job.hh"


void open_server (int port);   // function which receive connections from slaves
void *accept_client (void *args);   // thread function used to receive connections from clients
void signal_listener (int);   // thread function used to communicate with connected nodes
void run_job (char* buf_content, master_job* thejob);   // run submitted job
master_job* find_jobfromid (int id);   // find and return job pointer fro its jobid

#endif
