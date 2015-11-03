#include "slave.hh"
#include "slave_job.hh"
#include "slave_task.hh"
#include "../cache_slave/cache_slave.hh"

#include <iostream>
#include <thread>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

void block_until_event();
void close_and_exit();

char read_buf[BUF_SIZE];
char write_buf[BUF_SIZE];

int masterfd = -1;
int buffersize = 8388608; // 8 MB buffer size
Logger* logger;

vector<slave_job*> running_jobs; // a vector of job, one or multiple tasks of which are running on this slave node
vector<slave_task*> running_tasks; // a vector of running tasks

// main {{{
int main (int argc, char** argv)
{
    Settings setted;
    setted.load();
    int port = setted.get<int>("network.port_mapreduce");
    string master_addr = setted.get<string>("network.master");
    string logname     = setted.get<string> ("log.name");
    string logtype     = setted.get<string> ("log.type");
    logger             = Logger::connect(logname, logtype);

    connect_to_server (master_addr.c_str(), port); // connect to master
   
    auto cache_thread = std::thread ([&] () {
        sleep (5);
        Cache_slave cache_slave;
        cache_slave.connect ();
        cache_slave.run_server (); 
    });

    signal_listener();

    cache_thread.join();
    Logger::disconnect(logger);

    return EXIT_SUCCESS;
}
// }}}
// Connect to Server {{{
void connect_to_server (const char* host, unsigned short port)
{
    struct sockaddr_in serveraddr = {0};
    
    masterfd = socket (AF_INET, SOCK_STREAM, 0); //SOCK_STREAM -> tcp
    
    if (masterfd < 0)
        logger->panic ("[slave]Openning socket failed");
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons (port);
    inet_pton (AF_INET, host, &serveraddr.sin_addr);

    connect (masterfd, (struct sockaddr *) &serveraddr, sizeof (serveraddr));
    if (masterfd < 0)
        logger->panic ("[slave]Connecting to master failed");
    
    // set master socket to be non-blocking socket to avoid deadlock
    fcntl (masterfd, F_SETFL, O_NONBLOCK);
    setsockopt (masterfd, SOL_SOCKET, SO_SNDBUF, &buffersize, (socklen_t) sizeof (buffersize));
    setsockopt (masterfd, SOL_SOCKET, SO_RCVBUF, &buffersize, (socklen_t) sizeof (buffersize));
}
// }}}
// main loop {{{
void signal_listener()
{
    char* tokenbuf;
    int readbytes = 0;
    struct timeval time_start;
    struct timeval time_end;
    gettimeofday (&time_start, NULL);
    gettimeofday (&time_end, NULL);
    
    while (true)
    {
        block_until_event ();
        readbytes = nbread (masterfd, read_buf);
        
        if (readbytes == 0)     //connection closed from master
        {
            logger->error ("[slave]Connection from master is abnormally closed");
            close_and_exit ();
        }
        else if (readbytes < 0) {} // do nothing
        else                       // signal arrived from master
        {
            if (strncmp (read_buf, "whoareyou", 9) == 0)
            {
                snprintf (write_buf, BUF_SIZE, "slave");
                nbwrite (masterfd, write_buf);
            }
            else if (strncmp (read_buf, "close", 5) == 0)
            {
                logger->info ("[slave]Close request from master");
                close_and_exit ();
            }
            else if (strncmp (read_buf, "tasksubmit", 10) == 0) // launch the forwarded task
            {
                slave_job* thejob = NULL;
                slave_task* thetask = NULL;

                int jobid, taskid, argc;
                char *token, type [64], remaining[256];

                sscanf (read_buf, "%*s %i %i %s %i %[^\n]", &jobid, &taskid, type, &argc, remaining);

                thejob = find_jobfromid (jobid);

                if (!thejob)                                  // if any task in this job are not running in this slave
                {
                    thejob = new slave_job (jobid, masterfd);
                    running_jobs.push_back (thejob);
                }

                thetask = new slave_task (taskid);            // the status is running by default
                running_tasks.push_back (thetask);            // add to the running_tasks vector
                thejob->add_task (thetask);                   // add this task in 'thejob'

                logger->debug ("Capture task with args: %i %i %s %i %s", jobid, taskid, type, argc, remaining);
                
                if (strncmp (type, "MAP", 3) == 0)
                {
                    thetask->set_taskrole (MAP);
                    thetask->set_argcount (argc);

                    char** values = new char*[argc];
                    for (int i = 0; i < argc; i++)
                    {
                        token = strtok_r (remaining, " ", &tokenbuf);
                        values[i] = strdup (token);
                    }
                    
                    thetask->set_argvalues (values);
                    
                    while (true) // read messages from master until getting Einput
                    {
                        readbytes = nbread (masterfd, read_buf);
                        
                        if (readbytes == 0)
                        {
                            logger->error ("[slave]Connection from master is abnormally closed");
                        }
                        else if (readbytes < 0) { continue; }
                        else     // a message
                        {
                            if (strncmp (read_buf, "inputpath", 9) == 0)
                            {
                                token = strtok_r (read_buf, " ", &tokenbuf);   // token <- "inputpath"
                                
                                while ( (token = strtok_r(NULL, " ", &tokenbuf)) )
                                  thetask->add_inputpath (token);
                            }
                            else if (strncmp (read_buf, "Einput", 6) == 0) { break; }
                            else
                            {
                                logger->error ("[slave]Unexpected message order from master");
                            }
                        }
                    }
                    
                    launch_task (thetask); // launch the forwarded task
                }
                else if (strncmp (type, "REDUCE", 6) == 0)
                {
                    thetask->set_taskrole (REDUCE);
                    thetask->set_argcount (argc);

                    char** values = new char*[argc];
                    for (int i = 0; i < argc; i++)
                    {
                        token = strtok_r (remaining, " ", &tokenbuf);;
                        values[i] = strdup (token);
                    }
                    
                    thetask->set_argvalues (values);
                    
                    while (true) // read messages from master
                    {
                        readbytes = nbread (masterfd, read_buf);
                        if (readbytes == 0)
                        {
                            logger->error ("[slave]Connection from master is abnormally closed");
                        }
                        else if (readbytes < 0) { continue; }
                        else                    { break; }
                    }
                    
                    token = strtok_r (read_buf, " ", &tokenbuf);      // <- "inputpath"
                    token = strtok_r (NULL, " ", &tokenbuf);          // <- first peer id
                    while (token)
                    {
                        thetask->peerids.push_back (atoi (token));
                        token = strtok_r (NULL, " ", &tokenbuf);      // <- numiblock
                        thetask->numiblocks.push_back (atoi (token));
                        token = strtok_r (NULL, " ", &tokenbuf);      // <- peerids
                    }

                    launch_task (thetask);                            // launch the forwarded task
                }
                else
                {
                    logger->error  ("Debugging: the task role is undefined well.");
                    thetask->set_taskrole (JOB);
                }
            }
            else
            {
                logger->error ("[slave]Undefined signal from master: %s size: %d", read_buf, readbytes);
            }
        }
        
        for (int i = 0; (unsigned) i < running_jobs.size(); i++) // check the running_jobs
        {
            // check if all tasks in the job are finished
            if (running_jobs[i]->get_numrunningtasks() == 0)     // all task is finished
            {
                // clear job from the vectors
                slave_job* deleted_job = running_jobs[i];
                running_jobs.erase (running_jobs.begin() + i);
                i--;
                delete deleted_job;
            }
        }
        
        // check message from tasks through pipe
        for (int i = 0; (unsigned) i < running_tasks.size(); i++)
        {
            readbytes = nbread (running_tasks[i]->get_readfd(), read_buf);
            
            if (readbytes == 0)     {} // ignore this case as default
            else if (readbytes < 0) { continue; }
            else
            {
                if (strncmp (read_buf, "complete", 8) == 0)
                {
                    if (running_tasks[i]->get_taskrole() == MAP)        // map task
                    {
                        logger->debug("Complete msg from app: %s", read_buf);

                        bzero(write_buf, BUF_SIZE);
                        snprintf (write_buf, BUF_SIZE, "peerids %i",
                          running_tasks[i]->get_job()->get_jobid());

                        char* token = strtok_r (read_buf, " ", &tokenbuf);    // receive peerids

                        while ( (token = strtok_r(NULL, " ", &tokenbuf)) )
                          strncat (write_buf, (" " + string(token)).c_str(), BUF_SIZE);

                        nbwrite (masterfd, write_buf);
                    }
                    
                    bzero(write_buf, BUF_SIZE);
                    snprintf (write_buf, BUF_SIZE, "terminate"); // send terminate message
                    nbwrite (running_tasks[i]->get_writefd(), write_buf);

                    running_tasks[i]->set_status (COMPLETED); // mark the task as completed
                }
                else if (strncmp (read_buf, "requestconf", 11) == 0) // parse all task configure
                {   
                    bzero(write_buf, BUF_SIZE);
                    snprintf (write_buf, BUF_SIZE, "taskconf %i %i",
                      running_tasks[i]->get_job()->get_jobid(),
                      running_tasks[i]->get_taskid());
                      
                    nbwrite (running_tasks[i]->get_writefd(), write_buf);

                    string message;
                    if (running_tasks[i]->get_taskrole() == MAP) // send input paths
                    {
                        message = "inputpath";
                        for (int iter = 0; iter < running_tasks[i]->get_numinputpaths(); iter++)
                        {
                            if (message.length() + running_tasks[i]->get_inputpath (iter).length() + 1 <= BUF_SIZE)
                            {
                                message.append (" ");
                                message.append (running_tasks[i]->get_inputpath (iter));
                            }
                            else
                            {
                                if (running_tasks[i]->get_inputpath (iter).length() + 10 > BUF_SIZE)
                                {
                                    logger->error ("The length of inputpath excceded the limit");
                                }
                                
                                // send message to slave
                                bzero(write_buf, BUF_SIZE);
                                strcpy (write_buf, message.c_str());
                                nbwrite (running_tasks[i]->get_writefd(), write_buf);
                                message = "inputpath ";
                                message.append (running_tasks[i]->get_inputpath (iter));
                            }
                            logger->debug ("Sending msg to child args: %s", message.c_str());
                        }
                        
                        // send remaining paths
                        if (message.length() > strlen ("inputpath "))
                        {
                            bzero (write_buf, BUF_SIZE);
                            strcpy (write_buf, message.c_str());
                            nbwrite (running_tasks[i]->get_writefd(), write_buf);
                        }
                        
                        // notify end of inputpaths
                        bzero (write_buf, BUF_SIZE);
                        strcpy (write_buf, "Einput");
                        nbwrite (running_tasks[i]->get_writefd(), write_buf);
                    }
                    else // send input paths
                    {
                        message = "inputpath";
                        for (int j = 0; (unsigned) j < running_tasks[i]->peerids.size(); j++)
                        {
                          message += " " + to_string (running_tasks[i]->peerids[j]);
                          message += " " + to_string (running_tasks[i]->numiblocks[j]);
                        }
                        
                        bzero (write_buf, BUF_SIZE); // notify end of inputpaths
                        strcpy (write_buf, message.c_str());
                        nbwrite (running_tasks[i]->get_writefd(), write_buf);
                    }
                }
                else
                {
                    logger->error ("[slave]Undefined message protocol from task [Message: %s]", read_buf);
                }
            }
        }
        
        // check task clear
        for (int i = 0; (unsigned) i < running_tasks.size(); i++)
        {
            if (waitpid (running_tasks[i]->get_pid(), & (running_tasks[i]->pstat), WNOHANG))         // waitpid returned nonzero
            {
                if (running_tasks[i]->get_status() == COMPLETED)     // successful termination
                {
                    snprintf (write_buf, BUF_SIZE, "taskcomplete jobid %i taskid %i", 
                      running_tasks[i]->get_job()->get_jobid(),
                      running_tasks[i]->get_taskid());

                    nbwrite (masterfd, write_buf);

                    // clear all to things related to this task
                    running_tasks[i]->get_job()->finish_task (running_tasks[i]);
                    delete running_tasks[i];
                    running_tasks.erase (running_tasks.begin() + i);
                    i--;
                }
                else
                {
                    logger->error ("task with taskid %d jobid %d pid: %d terminated abnormally", 
                      running_tasks[i]->get_taskid(),
                      running_tasks[i]->get_job()->get_jobid(),
                      running_tasks[i]->get_pid());

                    sleep (1);
                    // TODO: clear data structures for the task
                    // TODO: launch the failed task again
                }
            }
        }
        gettimeofday (&time_end, NULL);
    }

    close_and_exit ();
} //}}}
// launch_task {{{
void launch_task (slave_task* atask)
{
    pid_t pid;
    int fd1[2], fd2[2];                  // two set of fds between slave and task(1)
    pipe (fd1);                          // fd1[0]: slave read, fd1[1]: task write
    pipe (fd2);                          // fd2[0]: task read, fd2[1]: slave write

    fcntl (fd1[0], F_SETFL, O_NONBLOCK); // set pipe fds to be non-blocking to avoid deadlock
    fcntl (fd1[1], F_SETFL, O_NONBLOCK);
    fcntl (fd2[0], F_SETFL, O_NONBLOCK);
    fcntl (fd2[1], F_SETFL, O_NONBLOCK);

    atask->set_readfd (fd1[0]);          // set pipe fds
    atask->set_writefd (fd2[1]);

    pid = fork();
    switch (pid) {
      case 0: {
        int count = atask->get_argcount();
        char** args_src = atask->get_argvalues();
        char** args = new char*[count + 4];

        transform (args_src, args_src + count, args, strdup);

        asprintf (&args[count], "%i", fd2[0]);            // pass pipe fds
        asprintf (&args[count + 1], "%i", fd1[1]);
        args[count + 3] = NULL;                           // pass null to last parameter
        
        switch (atask->get_taskrole()) {
          case MAP:    args[count + 2] = strdup("MAP"); break;
          case REDUCE: args[count + 2] = strdup("REDUCE"); break;
          default:     logger->panic ("The role of the task is not defined in %s", __func__);
        }
          
        logger->info ("Launching task with args: %s %s %s %s", args[0], args[1], args[2], args[3]);
        if (execv (args[0], args) == EXIT_FAILURE) // launch the task with the passed arguments
            logger->panic ("execv failed");

      } break;

      case EXIT_FAILURE:
        logger->info ("Task could not have been started due to child process forking failure");
        break;

      default:
        close (fd2[0]); // close pipe fds for task side
        close (fd1[1]);
        atask->set_pid (pid); // register the pid of the task process
    }
} // }}}
// find_jobfromid {{{
slave_job* find_jobfromid (int id)
{
    for (int i = 0; (unsigned) i < running_jobs.size(); i++)
    {
        if (running_jobs[i]->get_jobid() == id)
        {
            return running_jobs[i];
        }
    }
    
    return NULL;
}
// }}}
// block_until_event {{{
void block_until_event() {
  struct timespec ts;
  ts.tv_sec  = 0;
  ts.tv_nsec = 10000000;

  sigset_t mask;
  sigemptyset (&mask);
  sigaddset (&mask, SIGCHLD);

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(masterfd, &fds); /* adds sock to the file descriptor set */

  for (auto& ijob : running_tasks)
    FD_SET(ijob->get_readfd(), &fds);

  pselect (masterfd+running_tasks.size(), &fds, NULL, NULL, &ts, &mask);
} // }}}
// close_and_exit() {{{
void close_and_exit() { 
  while (close (masterfd) == EXIT_FAILURE) {
    logger->info ("[slave]Closing socket failed");
    usleep (1000);  // sleeps for 1 milli second
  }

  logger->info ("[slave]Exiting slave...");
  exit (EXIT_SUCCESS);
} //}}}
