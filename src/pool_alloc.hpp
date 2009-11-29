
#ifndef __POOL_ALLOC_HPP__
#define __POOL_ALLOC_HPP__

// TODO: this allocator shares the freelist with the pooled
// memory. This saves space but might negatively affect caching
// behavior (compared to a freelist stored in a separate
// array). Investigate this further once we can test real workloads.

#include "utils.hpp"

template <class super_alloc_t>
struct pool_alloc_t : public super_alloc_t {
    pool_alloc_t(size_t _nobjects, size_t _object_size)
        : nobjects(_nobjects), object_size(_object_size)
    {
        check("Object size must be at least the size of a pointer.",
              object_size < sizeof(void*));
            
        // Allocate the memory we need
        size_t pool_size = nobjects * object_size;
        mem = super_alloc_t::malloc(pool_size);
        check("Could not allocate memory.", mem == NULL);

        // Fill up the free list
        free_list = (free_node_t*)mem;
        free_node_t *node = free_list;
        for(size_t i = 0; i < nobjects - 1; i++) {
            node->next = (free_node_t*)((char*)node + object_size);
            node = node->next;
        }
        node->next = NULL;
    }
    
    ~pool_alloc_t() {
        super_alloc_t::free(mem);
        mem = NULL;
    }

    void *malloc(size_t size) {
        //check("Could not allocate object of different size", size != object_size);
        void *addr = (void*)free_list;
        if(addr) {
            free_list = free_list->next;
        }
        return addr;
        // TODO: add debug code
    }

    void free(void *ptr) {
        ((free_node_t*)ptr)->next = free_list;
        free_list = (free_node_t*)ptr;
        // TODO: add debug code
    }

    bool in_range(void *ptr) {
        return ptr >= mem && ptr < (char*)mem + object_size * nobjects;
    }

private:
    size_t nobjects;
    size_t object_size;
    void *mem;

    struct free_node_t {
        free_node_t *next;
    };
    free_node_t *free_list;
};

#endif // __POOL_ALLOC_HPP__

