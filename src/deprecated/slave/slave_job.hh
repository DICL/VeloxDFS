#ifdef DHT_SLAVE

///////////////
// DHT_SLAVE //
///////////////

#ifndef _SLAVE_JOB_
#define _SLAVE_JOB_


#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <common/ecfs.hh>
#include "slave_task.hh"

using namespace std;

class slave_job
{
    private:
        int jobid;
        int masterfd;
        vector<slave_task*> tasks;
        vector<slave_task*> running_tasks;
        vector<slave_task*> completed_tasks;
        
    public:
        slave_job();
        slave_job (int id, int fd);
        ~slave_job();
        
        void set_jobid (int id);
        int get_jobid();
        void finish_task (slave_task* atask);
        slave_task* find_taskfromid (int id);
        int get_numtasks(); // tasks of this job assigned to this slave
        int get_numrunningtasks(); // running tasks of this job assigned to this slave
        int get_numcompletedtasks(); // completed tasks of this job assigned to this slave
        slave_task* get_completedtask (int index);
        void add_task (slave_task* atask);
        slave_task* get_task (int index);
};

void slave_job::finish_task (slave_task* atask)
{
    for (int i = 0; (unsigned) i < this->running_tasks.size(); i++)
    {
        if (this->running_tasks[i] == atask)
        {
            this->running_tasks.erase (this->running_tasks.begin() + i);
            this->completed_tasks.push_back (atask);
            atask->set_status (COMPLETED);
            return;
        }
    }
}

slave_job::slave_job()
{
    this->jobid = -1;
    masterfd = -1;
}

slave_job::slave_job (int id, int fd)
{
    this->jobid = id;
    masterfd = fd;
}

slave_job::~slave_job()
{
    // do nothing as default
}

void slave_job::set_jobid (int id)
{
    this->jobid = id;
}

int slave_job::get_jobid()
{
    return this->jobid;
}

slave_task* slave_job::find_taskfromid (int id)
{
    for (int i = 0; (unsigned) i < this->tasks.size(); i++)
    {
        if (this->tasks[i]->get_taskid() == id)
        {
            return this->tasks[i];
        }
    }
    
    cout << "No such a task with that index in this job assigned to this slave" << endl;
    return NULL;
}

int slave_job::get_numtasks()
{
    return this->tasks.size();
}

int slave_job::get_numrunningtasks()
{
    return this->running_tasks.size();
}

int slave_job::get_numcompletedtasks()
{
    return this->completed_tasks.size();
}

void slave_job::add_task (slave_task* atask)
{
    if (atask->get_status() != RUNNING)
    {
        cout << "Debugging: The added task is not at running state." << endl;
    }
    
    atask->set_job (this);
    this->tasks.push_back (atask);
    this->running_tasks.push_back (atask);   // tasks are in running state initially
}

slave_task* slave_job::get_task (int index)
{
    if ( (unsigned) index >= this->tasks.size())
    {
        cout << "Debugging: index of bound in the slave_job::get_task() function." << endl;
        return NULL;
    }
    else
    {
        return this->tasks[index];
    }
}

slave_task* slave_job::get_completedtask (int index)
{
    if ( (unsigned) index < completed_tasks.size())
    {
        return completed_tasks[index];
    }
    else
    {
        cout << "Debugging: Index out of range in the slave_job::get_completedtask() function" << endl;
        return NULL;
    }
}


// member functioin of slave_task class


slave_task::slave_task()
{
    this->taskid = -1;
    this->role = JOB; // this should be changed to MAP or REDUCE
    this->status = RUNNING; // the default is RUNNING because here is slave side
    this->pipefds[0] = -1;
    this->pipefds[1] = -1;
    this->argcount = -1;
    this->argvalues = NULL;
    this-> job = NULL;
    this->pid = 0;
}

slave_task::slave_task (int id)
{
    this->taskid = id;
    this->role = JOB; // this should be changed to MAP or REDUCE
    this->status = RUNNING; // the default is RUNNING because here is slave side
    this->pipefds[0] = -1;
    this->pipefds[1] = -1;
    this->argcount = -1;
    this->argvalues = NULL;
    this-> job = NULL;
    this->pid = 0;
}

slave_task::~slave_task()
{
    // close all pipe fds
    close (this->pipefds[0]);
    close (this->pipefds[1]);
    
    // delete argvalues
    if (argvalues != NULL)
    {
        for (int i = 0; i < this->argcount; i++)
        {
            delete[] argvalues[i];
        }
        
        delete[] argvalues;
    }
}

void slave_task::set_taskid (int id)
{
    this->taskid = id;
}

int slave_task::get_taskid()
{
    return this->taskid;
}

void slave_task::set_pid (int id)
{
    this->pid = id;
}

int slave_task::get_pid()
{
    return this->pid;
}

void slave_task::set_taskrole (mr_role arole)
{
    this->role = arole;
}

mr_role slave_task::get_taskrole()
{
    return this->role;
}

void slave_task::set_status (task_status astatus)
{
    this->status = astatus;
}

task_status slave_task::get_status()
{
    return this->status;
}

void slave_task::set_readfd (int fd)
{
    this->pipefds[0] = fd;
}

void slave_task::set_writefd (int fd)
{
    this->pipefds[1] = fd;
}

int slave_task::get_readfd()
{
    return this->pipefds[0];
}

int slave_task::get_writefd()
{
    return this->pipefds[1];
}

int slave_task::get_argcount()
{
    return this->argcount;
}

void slave_task::set_argcount (int num)
{
    this->argcount = num;
}

char** slave_task::get_argvalues()
{
    return this->argvalues;
}

void slave_task::set_argvalues (char** argv)
{
    this->argvalues = argv;
}

slave_job* slave_task::get_job()
{
    return this->job;
}

void slave_task::set_job (slave_job* ajob)
{
    this->job = ajob;
}

void slave_task::add_inputpath (string apath)
{
    inputpaths.push_back (apath);
}

string slave_task::get_inputpath (int index)
{
    if ( (unsigned) index >= inputpaths.size())
    {
        cout << "Index out of bound in the slave_task::get_inputpath() function." << endl;
        return "";
    }
    else
    {
        return inputpaths[index];
    }
}

int slave_task::get_numinputpaths()
{
    return this->inputpaths.size();
}

#endif

#else

///////////////////////////
// DHT_SLAVE NOT DEFINED //
///////////////////////////

#ifndef _SLAVE_JOB_
#define _SLAVE_JOB_


#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <common/ecfs.hh>
#include "slave_task.hh"

using namespace std;

class slave_job
{
    private:
        int jobid;
        vector<slave_task*> tasks;
        vector<slave_task*> running_tasks;
        vector<slave_task*> completed_tasks;

    public:
        slave_job();
        slave_job (int id);
        ~slave_job();

        void set_jobid (int id);
        int get_jobid();
        void finish_task (slave_task* atask);
        slave_task* find_taskfromid (int id);
        int get_numtasks(); // tasks of this job assigned to this slave
        int get_numrunningtasks(); // running tasks of this job assigned to this slave
        int get_numcompletedtasks(); // completed tasks of this job assigned to this slave
        slave_task* get_completedtask (int index);
        void add_task (slave_task* atask);
        slave_task* get_task (int index);
};

void slave_job::finish_task (slave_task* atask)
{
    for (int i = 0; (unsigned) i < this->running_tasks.size(); i++)
    {
        if (this->running_tasks[i] == atask)
        {
            this->running_tasks.erase (this->running_tasks.begin() + i);
            this->completed_tasks.push_back (atask);
            atask->set_status (COMPLETED);
            return;
        }
    }
}

slave_job::slave_job()
{
    this->jobid = -1;
}

slave_job::slave_job (int id)
{
    this->jobid = id;
}

slave_job::~slave_job()
{
    // do nothing as default
}

void slave_job::set_jobid (int id)
{
    this->jobid = id;
}

int slave_job::get_jobid()
{
    return this->jobid;
}

slave_task* slave_job::find_taskfromid (int id)
{
    for (int i = 0; (unsigned) i < this->tasks.size(); i++)
    {
        if (this->tasks[i]->get_taskid() == id)
        {
            return this->tasks[i];
        }
    }

    cout << "No such a task with that index in this job assigned to this slave" << endl;
    return NULL;
}

int slave_job::get_numtasks()
{
    return this->tasks.size();
}

int slave_job::get_numrunningtasks()
{
    return this->running_tasks.size();
}

int slave_job::get_numcompletedtasks()
{
    return this->completed_tasks.size();
}

void slave_job::add_task (slave_task* atask)
{
    if (atask->get_status() != RUNNING)
    {
        cout << "Debugging: The added task is not at running state." << endl;
    }

    atask->set_job (this);
    this->tasks.push_back (atask);
    this->running_tasks.push_back (atask);   // tasks are in running state initially
}

slave_task* slave_job::get_task (int index)
{
    if ( (unsigned) index >= this->tasks.size())
    {
        cout << "Debugging: index of bound in the slave_job::get_task() function." << endl;
        return NULL;
    }
    else
    {
        return this->tasks[index];
    }
}

slave_task* slave_job::get_completedtask (int index)
{
    if ( (unsigned) index < completed_tasks.size())
    {
        return completed_tasks[index];
    }
    else
    {
        cout << "Debugging: Index out of range in the slave_job::get_completedtask() function" << endl;
        return NULL;
    }
}


// member functioin of slave_task class


slave_task::slave_task()
{
    this->taskid = -1;
    this->role = JOB; // this should be changed to MAP or REDUCE
    this->status = RUNNING; // the default is RUNNING because here is slave side
    this->pipefds[0] = -1;
    this->pipefds[1] = -1;
    this->argcount = -1;
    this->argvalues = NULL;
    this-> job = NULL;
    this->pid = 0;
}

slave_task::slave_task (int id)
{
    this->taskid = id;
    this->role = JOB; // this should be changed to MAP or REDUCE
    this->status = RUNNING; // the default is RUNNING because here is slave side
    this->pipefds[0] = -1;
    this->pipefds[1] = -1;
    this->argcount = -1;
    this->argvalues = NULL;
    this-> job = NULL;
    this->pid = 0;
}

slave_task::~slave_task()
{
    // close all pipe fds
    close (this->pipefds[0]);
    close (this->pipefds[1]);

    // delete argvalues
    if (argvalues != NULL)
    {
        for (int i = 0; i < this->argcount; i++)
        {
            delete[] argvalues[i];
        }

        delete[] argvalues;
    }
}

void slave_task::set_taskid (int id)
{
    this->taskid = id;
}

int slave_task::get_taskid()
{
    return this->taskid;
}

void slave_task::set_pid (int id)
{
    this->pid = id;
}

int slave_task::get_pid()
{
    return this->pid;
}

void slave_task::set_taskrole (mr_role arole)
{
    this->role = arole;
}

mr_role slave_task::get_taskrole()
{
    return this->role;
}

void slave_task::set_status (task_status astatus)
{
    this->status = astatus;
}

task_status slave_task::get_status()
{
    return this->status;
}

void slave_task::set_readfd (int fd)
{
    this->pipefds[0] = fd;
}

void slave_task::set_writefd (int fd)
{
    this->pipefds[1] = fd;
}

int slave_task::get_readfd()
{
    return this->pipefds[0];
}

int slave_task::get_writefd()
{
    return this->pipefds[1];
}

int slave_task::get_argcount()
{
    return this->argcount;
}

void slave_task::set_argcount (int num)
{
    this->argcount = num;
}

char** slave_task::get_argvalues()
{
    return this->argvalues;
}

void slave_task::set_argvalues (char** argv)
{
    this->argvalues = argv;
}

slave_job* slave_task::get_job()
{
    return this->job;
}

void slave_task::set_job (slave_job* ajob)
{
    this->job = ajob;
}

void slave_task::add_inputpath (string apath)
{
    inputpaths.push_back (apath);
}

string slave_task::get_inputpath (int index)
{
    if ( (unsigned) index >= inputpaths.size())
    {
        cout << "Index out of bound in the slave_task::get_inputpath() function." << endl;
        return "";
    }
    else
    {
        return inputpaths[index];
    }
}

int slave_task::get_numinputpaths()
{
    return this->inputpaths.size();
}

#endif

#endif
