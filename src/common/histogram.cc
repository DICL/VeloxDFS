#include "histogram.hh"

// Histogram() {{{
Histogram::Histogram() {
  numbin = -1;
  numserver = -1;
  boundaries = NULL;
  querycount = NULL;
}
// }}}
// Histogram(int, int) {{{
Histogram::Histogram(int numserver, int numbin) {
  boundaries = NULL;
  querycount = NULL;
  this->numserver = numserver;
  this->numbin = numbin;
  boundaries = new uint32_t[numserver];
  querycount = new double[numbin];
  // initialize
  this->initialize();
}
// }}}
// ~Histogram() {{{
Histogram::~Histogram() {
  if (boundaries != NULL) {
    delete[] boundaries;
  }
  if (querycount != NULL) {
    delete[] querycount;
  }
}
// }}}
// initialize {{{
void Histogram::initialize() {
  uint32_t max = MAX_UINT;
  // initialize the boundary
  for (int i = 0; i < numserver - 1; i++) //boundaries[i] = (inf/10) * i
    boundaries[i] = (uint32_t) ( ( (double) max / (double) numserver) * ( (double) (i + 1)));
  boundaries[numserver - 1] = max;
  // initialize the query count
  for (int i = 0; i < numbin; i++) {
    querycount[i] = 1.0 / (double) numbin;
  }
}
// }}}
// init_count {{{
void Histogram::init_count() {
  for (int i = 0; i < numbin; i++) {
    querycount[i] = 1.0 / (double) numbin;
  }
}
// }}}
// set_numbin {{{
void Histogram::set_numbin (int num) {
  numbin = num;
  if (querycount != NULL) {
    delete querycount;
  }
  querycount = new double[num];
}
// }}}
// get_numbin {{{
int Histogram::get_numbin() {
  return numbin;
}
// }}}
// set_numserver {{{
void Histogram::set_numserver (int num) {
  numserver = num;
  if (boundaries != NULL) {
    delete boundaries;
  }
  boundaries = new uint32_t[num];
}
// }}}
// get_numserver {{{
int Histogram::get_numserver() {
  return numserver;
}
// }}}
// set_boundary {{{
void Histogram::set_boundary (int net_id, uint32_t boundary) {
  boundaries[net_id] = boundary;
}
// }}}
// get_boundary {{{
uint32_t Histogram::get_boundary(int net_id) {    // the net_id starts from 0
  if (net_id >= numserver) {
    cout << "[Histogram]net_id requested is out of range" << endl;
    return -1;
  } else {
    return boundaries[net_id];
  }
}
// }}}
// set_count {{{
void Histogram::set_count (int net_id, double count) {
  querycount[net_id] = count;
}
// }}}
// get_count {{{
double Histogram::get_count (int net_id) {
  if (net_id >= numbin) {
    cout << "[Histogram] net_id requested is out of range" << endl;
    return -1.0;
  } else {
    return querycount[net_id];
  }
  return -1.0;
}
// }}}
// updateboundary {{{
void Histogram::updateboundary() {  // update the boundary according to the query counts
  // sum up the count of all bin and divide it by number of servers(query per server)
  double qps = 0.0;              // query per server
  double temp = 0.0;
  double stmeter = 0.0;
  int j = 0;
  for (int i = 0; i < numbin; i++) {
    qps += querycount[i];
  }
  qps = qps / numserver;
  // calculate the new boundary of each server(except the boundary of last server)
  for (int i = 0; i < numserver - 1; i++) {
    while (temp + querycount[j] * (1.0 - stmeter) < qps) {
      temp += querycount[j] * (1.0 - stmeter);
      j++;
      if (j >= numbin) {
        j--;
        break;
      }
      stmeter = 0.0;
    }
    if (stmeter == 0.0) {
      stmeter = (qps - temp) / querycount[j];
    } else {
      stmeter += (qps - temp) / querycount[j];
    }
    boundaries[i] = ( (double) MAX_UINT) * ( ( (double) j + stmeter) / ( (double) numbin));
    temp = 0.0;
  }
  // re-initialize the count of each bin
  // don't need to do because it is EM-KDE and old queries are automatically faded out
}
// }}}
// count_query {{{
int Histogram::count_query(uint32_t query) {
  // accumulate the query count to the target bin
  int net_id = (int) ( ( (double) query / (double) MAX_UINT) * (double) numbin);
  if (net_id == numbin) {
    net_id--;
  } else if (net_id > numbin) {
    cout << "[Histogram] Debugging: Cannot find net_id of requested query." << endl;
  }
  // alpha adjustment
  for (int i = 0; i < numbin; i++) {
    querycount[i] *= (1.0 - ALPHA);
  }
  // accumulate the alpha value (approximated kernel density estimation with KERNELWIDTH)
  int net_idstart;
  int net_idend;
  double increment;
  if (net_id - KERNELWIDTH < 0) {
    net_idstart = 0;
  } else {
    net_idstart = net_id - KERNELWIDTH;
  }
  if (net_id + KERNELWIDTH > numbin - 1) {
    net_idend = numbin - 1;
  } else {
    net_idend = net_id + KERNELWIDTH;
  }
  increment = ALPHA / ( (double) (net_idend - net_idstart + 1));
  for (int i = net_idstart; i <= net_idend; i++) {
    querycount[i] += increment;
  }
  return net_id; // returns the target net_id as representative Histogram bin
}
// }}}
// get_net_id {{{
int Histogram::get_net_id(uint32_t query) {  // return the server net_id range of which includes the input query
  for (int i = 0; i < numserver; i++) {
    if (query <= boundaries[i]) {
      return i;
    }
  }
  cout << "[Histogram] Debugging: Cannot find net_id of requested query." << endl;
  return -1;
}
// }}}
// random_within_boundaries {{{
uint32_t Histogram::random_within_boundaries (int net_id) {
  srand((uint32_t)time(NULL));
  int which_server = net_id;
  uint32_t lower_boundary ;
  if (which_server != 0)
    lower_boundary = boundaries[which_server-1];
  else 
    lower_boundary = boundaries[numserver-1];
  uint32_t upper_boundary = boundaries[which_server];
  uint32_t range = upper_boundary - lower_boundary;
  uint32_t result = rand() % range + lower_boundary;
  return result;
}
// }}}
