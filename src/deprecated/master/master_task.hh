#ifndef __MASTER_TASK__
#define __MASTER_TASK__

#include <iostream>
#include <common/ecfs.hh>

using namespace std;

class master_job;

class master_task
{
    private:
        int taskid;
        master_job* job;
        mr_role role; // MAP or REDUCE
        task_status status;
        
        // FOR MAP
        vector<string> inputpaths;// a vector of inputpaths. inputpaths can be multiple
        
    public:
        // FOR REDUCER
        vector<int> peerids; // list of peer ids in which the idata is located
        vector<int> numiblocks; // list of number of blocks for each peer
        
        master_task();
        master_task (mr_role arole);
        master_task (master_job* ajob);
        master_task (master_job* ajob, mr_role arole);
        
        int gettaskid();
        void settaskid (int num);
        master_job* get_job();
        void set_job (master_job* ajob);
        mr_role get_taskrole();
        void set_taskrole (mr_role arole);
        int get_numinputpaths();
        string get_inputpath (int index);
        void add_inputpath (string path);
        void set_status (task_status astatus);
        task_status get_status();
};

#endif
