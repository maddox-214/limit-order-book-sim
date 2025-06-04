#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include "Order.h"
#include <cstddef>
#include <vector>

// A simple fixed-size memory pool for Order objects.
// Preallocates N Orders up front and dishes them out on demand.
// Frees are recycled into a singly-linked free list.
class MemoryPool {
public:
    // Construct a pool that can hold up to capacity Orders.
    explicit MemoryPool(size_t capacity);
    ~MemoryPool();

    // Allocate one Order. Returns nullptr if exhausted.
    Order* allocate();

    // Return an Order to the pool.
    void deallocate(Order* order);

private:
    std::vector<Order*>   blocks_;      // raw pointers to allocated blocks
    Order*                free_list_;   // singly-linked list of free Orders
    size_t                capacity_;
};

#endif 
