#pragma once
#include <iostream>
#include <common/ecfs.hh>
#include <ctime>
#include <cstdint>

#define MAX_UINT 4294967295

using namespace std;

class Histogram {
    private:
        int numserver; // number of server
        int numbin; // number of bin -> number of histogram bin
        // int digit; // number of digits to represent the problem space
        double* querycount; // the data access count to each
        uint32_t* boundaries; // the net_id of end point of each node
        
    public:
        Histogram(); // constructs an uninitialized object
        Histogram (int numserver, int numbin);   // number of bin and number of digits
        ~Histogram();
        void initialize(); // partition the problem space equally to each bin
        void init_count(); // initialize the all query counts to zero
        void set_numbin (int num);
        int get_numbin();
        void set_numserver (int num);
        int get_numserver();
        void set_boundary (int net_id, uint32_t boundary);
        uint32_t get_boundary (int net_id);
        void set_count (int net_id, double count);
        double get_count (int net_id);
        void updateboundary();
        int count_query (uint32_t query);
        int get_net_id (uint32_t query);   // return the dedicated node net_id of query
        uint32_t random_within_boundaries(int);
};
