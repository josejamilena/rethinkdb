
#ifndef __CONFIG__H__
#define __CONFIG__H__

// Max concurrent IO requests per event queue
#define MAX_CONCURRENT_IO_REQUESTS                300

// A hint about the average number of concurrent network events
#define CONCURRENT_NETWORK_EVENTS_COUNT_HINT      300

// Defines the maximum size of the batch of IO events to process on
// each loop iteration. A larger number will increase throughput but
// decrease concurrency
#define MAX_IO_EVENT_PROCESSING_BATCH_SIZE        10


// Defines the maximum number of allocators in
// dynamic_pool_alloc_t. Since the size of each allocator is doubled
// every time, a reasonably small number should be sufficient.
#define DYNAMIC_POOL_MAX_ALLOCS                   20

// Initial number of objects in the first dynamic pool allocator.
#define DYNAMIC_POOL_INITIAL_NOBJECTS             100


#endif // __CONFIG__H__

