#pragma once

#ifndef BUF_SIZE
#define BUF_SIZE (8*1024) // determines maximum size of a record
#endif

#ifndef BUF_THRESHOLD
#define BUF_THRESHOLD (7*1024) // the buffer flush threshold
#endif

#ifndef BUF_CUT
#define BUF_CUT 512
#endif

#ifndef CACHESIZE
#define CACHESIZE (1*1024*1024*1024) // 1 GB of cache size
#endif

#ifndef BLOCKSIZE
#define BLOCKSIZE (512*1024) // 512 KB sized block <- should be multiple of BUF_SIZE
#endif

#ifndef IBLOCKSIZE
#define IBLOCKSIZE (64*1024*1024) // size of intermediate flush threshold
#endif

// EM-KDE
#ifndef ALPHA
#define ALPHA 0.001
#endif

#ifndef NUMBIN
#define NUMBIN 100 // number of histogram bins in the EM-KDE scheduling
#endif

#ifndef UPDATEINTERVAL
#define UPDATEINTERVAL 5000 // update interval in msec
#endif

#ifndef KERNELWIDTH
#define KERNELWIDTH 2 // number of bins affected by count_query() function: 1 + 2*KERNELWIDTH (except the boundary bins)
#endif

#ifndef MAP_SLOT
#define MAP_SLOT 16
#endif

#ifndef REDUCE_SLOT
#define REDUCE_SLOT 16
#endif

#ifndef BACKLOG
#define BACKLOG 16384
#endif
