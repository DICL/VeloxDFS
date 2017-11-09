#ifndef __HISTOGRAM__
#define __HISTOGRAM__

#include <iostream>
#include <ctime>

#define MAX_UINT 4294967295

// EM-KDE
#ifndef ALPHA
#define ALPHA 0.001
#endif

#ifndef KERNELWIDTH
#define KERNELWIDTH 2 // number of bins affected by count_query() function: 1 + 2*KERNELWIDTH (except the boundary bins)
#endif

using namespace std;

class Histogram
{
    private:
        int numserver; // number of server
        int numbin; // number of bin -> number of histogram bin
        // int digit; // number of digits to represent the problem space
        double* querycount; // the data access count to each
        uint32_t* boundaries; // the index of end point of each node
        
    public:
        Histogram(); // constructs an uninitialized object
        Histogram (int numserver, int numbin);   // number of bin and number of digits
        ~Histogram();
        
        void initialize(); // partition the problem space equally to each bin
        void init_count(); // initialize the all query counts to zero
        unsigned get_boundary (int index);
        void set_boundary (int index, unsigned int boundary);
        double get_count (int index);
        void set_count (int index, double count);
        int get_index (unsigned query);   // return the dedicated node index of query
        int count_query (unsigned query);
        void updateboundary();
        uint32_t random_within_boundaries(unsigned int);
        
        void set_numbin (int num);
        int get_numbin();
        
        void set_numserver (int num);
        int get_numserver();
};

#endif
