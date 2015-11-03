#ifndef _SLAVE_TASK_
#define _SLAVE_TASK_

#include <iostream>
#include <common/ecfs.hh>
#include <vector>
#include <string>

using namespace std;

class slave_job;

class slave_task
{
    private:
        int taskid;
        int pid;
        mr_role role; // MAP or REDUCE
        task_status status;
        int pipefds[2]; // 0 for read, 1 for write
        int argcount;
        char** argvalues;
        slave_job* job;
        vector<string> inputpaths; // a vector of inputpaths. inputpaths can be multiple
        
    public:
        vector<int> peerids;
        vector<int> numiblocks;
        slave_task();
        slave_task (int id);
        ~slave_task();
        
        int pstat; // status value for waitpid()
        
        void set_taskid (int id);
        int get_taskid();
        void set_pid (int id);
        int get_pid();
        void set_taskrole (mr_role arole);
        mr_role get_taskrole();
        void set_status (task_status astatus);
        task_status get_status();
        void set_readfd (int fd);
        void set_writefd (int fd);
        int get_readfd();
        int get_writefd();
        int get_argcount();
        void set_argcount (int num);
        char** get_argvalues();
        void set_argvalues (char** argv);
        slave_job* get_job();
        void set_job (slave_job* ajob);
        void add_inputpath (string apath);
        string get_inputpath (int index);
        int get_numinputpaths();
};

#endif
