#ifndef __IWFREQUEST__
#define __IWFREQUEST__

class iwfrequest
{
    private:
        int received;
        int requested;
        int jobid;
        
    public:
        vector<int> peerids;
        vector<int> numblocks;
        
        int get_jobid();
        bool is_finished();
        iwfrequest (int ajobid);
        void add_request (int num);
        void add_receive (int index, int numblock);
};

iwfrequest::iwfrequest (int ajobid)
{
    jobid = ajobid;
    received = 0;
    requested = 0;
}

void iwfrequest::add_request (int num)
{
    peerids.push_back (num);
    numblocks.push_back (-1);
    requested++;
}

int iwfrequest::get_jobid()
{
    return jobid;
}

void iwfrequest::add_receive (int index, int numblock)
{
    for (int i = 0; (unsigned) i < peerids.size(); i++)
    {
        if (peerids[i] == index)
        {
            numblocks[i] = numblock;
            received++;
            return;
        }
    }
    
    cout << "check add_receive()" << endl;
}

bool iwfrequest::is_finished()
{
    if (received == requested)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif
