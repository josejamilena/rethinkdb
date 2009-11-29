
#ifndef __DYNAMIC_POOL_ALLOC_HPP__
#define __DYNAMIC_POOL_ALLOC_HPP__

#include <math.h>
#include "config.hpp"
#include "utils.hpp"

// TODO: We double the size of the allocator every time, which means
// we can use half the RAM or so until the super_alloc allocator
// request will fail. If we get to this point we should adjust the
// allocation factor (but it's not clear this will ever happen in
// practice).

template <class super_alloc_t>
struct dynamic_pool_alloc_t {
    dynamic_pool_alloc_t(size_t _object_size)
        : nallocs(1), smallest_free(0), object_size(_object_size)
        {
            check("Dynamic pool configuration error", DYNAMIC_POOL_MAX_ALLOCS < nallocs);
            allocs[0] = new super_alloc_t(compute_alloc_nobjects(0), object_size);
            check("Could not allocate memory in dynamic pool", allocs[0] == NULL);
        }
    ~dynamic_pool_alloc_t() {
        for(int i = 0; i < nallocs; i++) {
            delete allocs[i];
        }
    }

    void *malloc(size_t size) {
        // Try to allocate from the smallest allocator that was last free
        void *ptr = allocs[smallest_free]->malloc(size);
        // If we couldn't allocate memory, we have to do more work
        if(!ptr) {
            // First, try to go through existing allocators
            for(unsigned int i = smallest_free; i < nallocs; i++) {
                ptr = allocs[i]->malloc(size);
                if(ptr) {
                    smallest_free = i;
                    break;
                }
            }
            // If we still couldn't allocate, create a new allocator (if we can)
            if(!ptr && nallocs + 1 <= DYNAMIC_POOL_MAX_ALLOCS) {
                nallocs++;
                smallest_free = nallocs - 1;
                allocs[smallest_free] =
                    new super_alloc_t(compute_alloc_nobjects(smallest_free),
                                      object_size);
                ptr = allocs[smallest_free]->malloc(size);
            }
        }
        return ptr;
    }

    void free(void *ptr) {
        for(int i = 0; i < nallocs; i++) {
            if(allocs[i]->in_range(ptr)) {
                allocs[i]->free(ptr);
                if(i < smallest_free) {
                    smallest_free = i;
                }
                break;
            }
        }
        // TODO: add debug code for when ptr isn't in any of the
        // allocators
    }

    // This function should be called periodically (probably on
    // timer), if we want the allocator to release unused memory back
    // to the system. Otherwise, the allocator will use as much memory
    // as was required during peak utilization. Note that this isn't
    // strictly a garbage collector, as the garbage doesn't
    // accumilate.
    void relelase_unused_memory() {
        // TODO: collect as many allocators from the end as we can.
    }

private:
    super_alloc_t* allocs[DYNAMIC_POOL_MAX_ALLOCS];
    unsigned int nallocs;
    unsigned int smallest_free;
    size_t object_size;

    unsigned int compute_alloc_nobjects(int alloc) {
        int n = DYNAMIC_POOL_INITIAL_NOBJECTS * pow(2, alloc);
        return n;
    }
};

#endif // __DYNAMIC_POOL_ALLOC_HPP__

