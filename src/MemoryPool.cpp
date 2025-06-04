#include "MemoryPool.h"
#include <new>      

MemoryPool::MemoryPool(size_t capacity)
    : free_list_(nullptr), capacity_(capacity)
{
    // Allocate a single contiguous block of capacity Orders
    Order* block = static_cast<Order*>(::operator new[](capacity * sizeof(Order)));
    blocks_.push_back(block);

    // Initialize free_list_ to chain all slots
    for (size_t i = 0; i < capacity; ++i) {
        Order* obj = block + i;
        obj->next_free = free_list_;
        free_list_ = obj;
    }
}

MemoryPool::~MemoryPool()
{
    // Call destructors (if needed) and free raw memory
    for (auto block : blocks_) {
        ::operator delete[](block);
    }
    blocks_.clear();
    free_list_ = nullptr;
}

Order* MemoryPool::allocate()
{
    if (!free_list_) return nullptr;
    Order* obj = free_list_;
    free_list_ = free_list_->next_free;
    // Placement-new is not strictly needed since we use POD style; but if initialization is needed:
    // new (obj) Order();
    obj->next_free = nullptr;
    return obj;
}

void MemoryPool::deallocate(Order* order)
{
    if (!order) return;
    // Explicitly reset fields if you want, or just push back onto free list
    order->next_free = free_list_;
    free_list_ = order;
}
