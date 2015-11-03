#ifndef __MASTER_JOB__
#define __MASTER_JOB__

#include <iostream>
#include <common/ecfs.hh>
#include <string>
#include <vector>
#include <set>
#include "master_task.hh"
#include "connslave.hh"

using namespace std;

enum mapstatus
{
    TASK_FINISHED,
    REQUEST_SENT,
    RESPOND_RECEIVED
};
class master_job
{
    private:
        int jobid;
        int jobfd;
        int argcount;
        int nummap;
        int numreduce;
        char** argvalues; // contains program name
        job_stage stage;
        vector<string> inputpaths;
        vector<master_task*> tasks;
        vector<master_task*> waiting_tasks;
        vector<master_task*> running_tasks;
        vector<master_task*> completed_tasks;
        
    public:
        enum mapstatus status;
        set<int> peerids;
        vector<int> numiblocks; // the order is matched with peerids(set)
        
        int scheduled;
        
        master_job();
        master_job (int id, int fd);
        ~master_job();
        
        void setjobid (int num);
        int getjobid();
        void setjobfd (int num);
        int getjobfd();
        void setnummap (int num);
        int getnummap();
        void setnumreduce (int num);
        int getnumreduce();
        void setargcount (int num);
        int getargcount();
        void setargvalues (char** values);
        char** getargvalues();
        string getargvalue (int index);
        void add_inputpath (string path);
        string get_inputpath (int index);
        int get_numinputpaths();
        void add_task (master_task* atask);
        master_task* get_task (int index);
        master_task* get_waitingtask (int index);
        int get_numtasks();
        int get_numwaiting_tasks();
        int get_numrunning_tasks();
        int get_numcompleted_tasks();
        job_stage get_stage();
        void set_stage (job_stage astage);
        master_task* get_lastwaitingtask();
        void schedule_task (master_task* atask, connslave* aslave);
        void finish_task (master_task* atask, connslave* aslave);
        master_task* find_taskfromid (int id);
        
};

master_job::master_job()
{
    this->jobid = -1;
    this->jobfd = -1;
    this->nummap = 0;
    this->numreduce = 0;
    this->argcount = -1;
    this->argvalues = NULL;
    this->stage = INITIAL_STAGE;
    status = TASK_FINISHED;
}

master_job::master_job (int id, int fd)
{
    scheduled = 0;
    this->jobid = id;
    this->jobfd = fd;
    this->nummap = 0;
    this->numreduce = 0;
    this->argcount = -1;
    this->argvalues = NULL;
    this->stage = INITIAL_STAGE;
    status = TASK_FINISHED;
}

master_job::~master_job()
{
    if (argvalues != NULL)
    {
        for (int i = 0; i < this->argcount; i++)
        {
            delete[] argvalues[i];
        }
    }
    
    delete[] argvalues;
    
    // delete all the tasks of the job
    for (int i = 0; (unsigned) i < tasks.size(); i++)
    {
        delete tasks[i];
    }
    
    close (jobfd);
}

void master_job::setjobid (int num)
{
    this->jobid = num;
}

int master_job::getjobid()
{
    return this->jobid;
}

void master_job::setjobfd (int num)
{
    this->jobfd = num;
}

int master_job::getjobfd()
{
    return this->jobfd;
}

void master_job::setargcount (int num)
{
    this->argcount = num;
}

int master_job::getargcount()
{
    return this->argcount;
}

void master_job::setargvalues (char** values)
{
    this->argvalues = values;
}

char** master_job::getargvalues()
{
    return this->argvalues;
}

void master_job::add_inputpath (string path)
{
    this->inputpaths.push_back (path);
}

string master_job::get_inputpath (int index)
{
    if ( (unsigned) index < inputpaths.size())
    {
        return this->inputpaths[index];
    }
    else
    {
        cout << "index out of bound in master_job::get_inputpath()" << endl;
        return "";
    }
}

int master_job::get_numinputpaths()
{
    return this->inputpaths.size();
}

void master_job::add_task (master_task* atask)
{
    // set the task id of the input task
    atask->settaskid (tasks.size());
    atask->set_job (this);
    this->tasks.push_back (atask);
    this->waiting_tasks.push_back (atask);
}

master_task* master_job::get_task (int index)
{
    if ( (unsigned) index >= tasks.size())
    {
        cout << "Debugging: index out of bound in the matser_job::get_task() function" << endl;
        return NULL;
    }
    else
    {
        return this->tasks[index];
    }
}

master_task* master_job::get_waitingtask (int index)
{
    if ( (unsigned) index >= waiting_tasks.size())
    {
        cout << "Debugging: index out of bound in the matser_job::get_waitingtask() function" << endl;
        cout << "index: " << index << ", waiting_tasks.size(): " << waiting_tasks.size() << endl;
        return NULL;
    }
    else
    {
        return waiting_tasks[index];
    }
}

int master_job::get_numtasks()
{
    return this->tasks.size();
}

master_task* master_job::get_lastwaitingtask()
{
    if (waiting_tasks.size() == 0)
    {
        return NULL;
    }
    else
    {
        return waiting_tasks.back();
    }
}

void master_job::schedule_task (master_task* atask, connslave* aslave)
{
    for (int i = 0; (unsigned) i < waiting_tasks.size(); i++)
    {
        if (waiting_tasks[i] == atask)
        {
            running_tasks.push_back (atask);
            aslave->add_runningtask (atask);
            atask->set_status (RUNNING);
            waiting_tasks.erase (waiting_tasks.begin() + i);
            return;
        }
    }
}

void master_job::finish_task (master_task* atask, connslave* aslave)
{
    for (int i = 0; (unsigned) i < running_tasks.size(); i++)
    {
        if (running_tasks[i] == atask)
        {
            completed_tasks.push_back (atask);
            aslave->remove_runningtask (atask);
            atask->set_status (COMPLETED);
            running_tasks.erase (running_tasks.begin() + i);
            return;
        }
    }
}

int master_job::get_numwaiting_tasks()
{
    return this->waiting_tasks.size();
}

int master_job::get_numrunning_tasks()
{
    return this->running_tasks.size();
}

int master_job::get_numcompleted_tasks()
{
    return this->completed_tasks.size();
}

master_task* master_job::find_taskfromid (int id)
{
    for (int i = 0; (unsigned) i < tasks.size(); i++)
    {
        if (tasks[i]->gettaskid() == id)
        {
            return tasks[i];
        }
    }
    
    cout << "There is no such a task with the id in master_job::find_taskfromid() function." << endl;
    return NULL;
}

void master_job::setnummap (int num)
{
    this->nummap = num;
}

int master_job::getnummap()
{
    return this->nummap;
}

void master_job::setnumreduce (int num)
{
    this->numreduce = num;
}

int master_job::getnumreduce()
{
    return this->numreduce;
}

string master_job::getargvalue (int index)
{
    if (index >= argcount)
    {
        cout << "Debugging: index out of bound in the master_job::getargvalue() function.";
        return "";
    }
    else
    {
        return this->argvalues[index];
    }
}

job_stage master_job::get_stage()
{
    return this->stage;
}

void master_job::set_stage (job_stage astage)
{
    this->stage = astage;
}


// member functions of master_task class


master_task::master_task()
{
    this->taskid = -1;
    this->job = NULL;
    this->role = JOB; // this should be changed to MAP or REDUCE
    this->status = WAITING;
}

master_task::master_task (mr_role arole)
{
    this->taskid = -1;
    this->job = NULL;
    this->role = arole;
    this->status = WAITING;
}

master_task::master_task (master_job* ajob)
{
    this->taskid = -1;
    this->job = ajob;
    this->role = JOB; // this should be changed to MAP ro REDUCE
    this->status = WAITING;
}

master_task::master_task (master_job* ajob, mr_role arole)
{
    this->taskid = -1;
    this->job = ajob;
    this->role = arole;
    this->status = WAITING;
}

int master_task::gettaskid()
{
    return this->taskid;
}

void master_task::settaskid (int num)
{
    this->taskid = num;
}

void master_task::add_inputpath (string path)
{
    this->inputpaths.push_back (path);
}

mr_role master_task::get_taskrole()
{
    return this->role;
}

int master_task::get_numinputpaths()
{
    return this->inputpaths.size();
}

string master_task::get_inputpath (int index)
{
    if ( (unsigned) index < this->inputpaths.size())
    {
        return this->inputpaths[index];
    }
    else if (index < 0)
    {
        cout << "Negative index in the master_task::get_inputpath() function." << endl;
        return "";
    }
    else
    {
        cout << "Index out of bound in the master_task::get_inputpath() function." << endl;
        return "";
    }
}

void master_task::set_status (task_status astatus)
{
    this->status = astatus;
}

task_status master_task::get_status()
{
    return this->status;
}

void master_task::set_job (master_job* ajob)
{
    this->job = ajob;
}

master_job* master_task::get_job()
{
    return this->job;
}

void master_task::set_taskrole (mr_role arole)
{
    this->role = arole;
}

#endif
