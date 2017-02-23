#include "histogram.hh"

Histogram::Histogram()
{
    numbin = -1;
    numserver = -1;
    boundaries = NULL;
    querycount = NULL;
}

Histogram::Histogram (int numserver, int numbin)
{
    boundaries = NULL;
    querycount = NULL;
    this->numserver = numserver;
    this->numbin = numbin;
    boundaries = new unsigned[numserver];
    querycount = new double[numbin];
    // initialize
    this->initialize();
}

Histogram::~Histogram()
{
    if (boundaries != NULL)
    {
        delete[] boundaries;
    }
    
    if (querycount != NULL)
    {
        delete[] querycount;
    }
}

void Histogram::initialize()
{
    unsigned max = MAX_UINT;
    
    // initialize the boundary
    for (int i = 0; i < numserver - 1; i++) //boundaries[i] = (inf/10) * i
      boundaries[i] = (uint32_t) ( ( (double) max / (double) numserver) * ( (double) (i + 1)));
    
    boundaries[numserver - 1] = max;
    
    // initialize the query count
    for (int i = 0; i < numbin; i++)
    {
        querycount[i] = 1.0 / (double) numbin;
    }
}

void Histogram::init_count()
{
    for (int i = 0; i < numbin; i++)
    {
        querycount[i] = 1.0 / (double) numbin;
    }
}

void Histogram::set_numbin (int num)
{
    numbin = num;
    
    if (querycount != NULL)
    {
        delete querycount;
    }
    
    querycount = new double[num];
}

int Histogram::get_numbin()
{
    return numbin;
}
void Histogram::set_numserver (int num)
{
    numserver = num;
    
    if (boundaries != NULL)
    {
        delete boundaries;
    }
    
    boundaries = new unsigned[num];
}

int Histogram::get_numserver()
{
    return numserver;
}

unsigned Histogram::get_boundary (int index)     // the index starts from 0
{
    if (index >= numserver)
    {
        cout << "[Histogram]Index requested is out of range" << endl;
        return -1;
    }
    else
    {
        return boundaries[index];
    }
}

void Histogram::set_boundary (int index, unsigned boundary)
{
    boundaries[index] = boundary;
}

double Histogram::get_count (int index)
{
    if (index >= numbin)
    {
        cout << "[Histogram]Index requested is out of range" << endl;
        return -1.0;
    }
    else
    {
        return querycount[index];
    }
    
    return -1.0;
}

void Histogram::set_count (int index, double count)
{
    querycount[index] = count;
}

void Histogram::updateboundary()   // update the boundary according to the query counts
{
    // sum up the count of all bin and divide it by number of servers(query per server)
    double qps = 0.0;              // query per server
    double temp = 0.0;
    double stmeter = 0.0;
    int j = 0;
    
    for (int i = 0; i < numbin; i++)
    {
        qps += querycount[i];
    }
    
    qps = qps / numserver;
    
    // calculate the new boundary of each server(except the boundary of last server)
    for (int i = 0; i < numserver - 1; i++)
    {
        while (temp + querycount[j] * (1.0 - stmeter) < qps)
        {
            temp += querycount[j] * (1.0 - stmeter);
            j++;
            
            if (j >= numbin)
            {
                j--;
                break;
            }
            
            stmeter = 0.0;
        }
        
        if (stmeter == 0.0)
        {
            stmeter = (qps - temp) / querycount[j];
        }
        else
        {
            stmeter += (qps - temp) / querycount[j];
        }
        
        boundaries[i] = ( (double) MAX_UINT) * ( ( (double) j + stmeter) / ( (double) numbin));
        temp = 0.0;
    }
    
    // re-initialize the count of each bin
    // don't need to do because it is EM-KDE and old queries are automatically faded out
}

int Histogram::count_query (unsigned query)
{
    // accumulate the query count to the target bin
    int index = (int) ( ( (double) query / (double) MAX_UINT) * (double) numbin);
    
    if (index == numbin)
    {
        index--;
    }
    else if (index > numbin)
    {
        cout << "[Histogram]Debugging: Cannot find index of requested query." << endl;
    }
    
    // alpha adjustment
    for (int i = 0; i < numbin; i++)
    {
        querycount[i] *= (1.0 - ALPHA);
    }
    
    // accumulate the alpha value (approximated kernel density estimation with KERNELWIDTH)
    int indexstart;
    int indexend;
    double increment;
    
    if (index - KERNELWIDTH < 0)
    {
        indexstart = 0;
    }
    else
    {
        indexstart = index - KERNELWIDTH;
    }
    
    if (index + KERNELWIDTH > numbin - 1)
    {
        indexend = numbin - 1;
    }
    else
    {
        indexend = index + KERNELWIDTH;
    }
    
    increment = ALPHA / ( (double) (indexend - indexstart + 1));
    
    for (int i = indexstart; i <= indexend; i++)
    {
        querycount[i] += increment;
    }
    
    return index; // returns the target index as representative Histogram bin
}

int Histogram::get_index (unsigned query)     // return the server index range of which includes the input query
{
    for (int i = 0; i < numserver; i++)
    {
        if (query <= boundaries[i])
        {
            return i;
        }
    }
    
    cout << "[Histogram]Debugging: Cannot find index of requested query." << endl;
    return -1;
}

// random_within_boundaries {{{
uint32_t Histogram::random_within_boundaries (uint32_t index) {
  srand((uint32_t)time(NULL));
  uint32_t which_server = index;
  uint32_t lower_boundary ;

  if (which_server != 0)
    lower_boundary = boundaries[which_server-1];
  else 
    lower_boundary = 0;

  uint32_t upper_boundary = boundaries[which_server];
  uint32_t range = upper_boundary - lower_boundary;
  uint32_t result = rand() % range + lower_boundary;

  return result;
}
// }}}
