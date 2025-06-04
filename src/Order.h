#ifndef ORDER_H
#define ORDER_H

#include <cstdint>

// Represents a single order in the book
struct Order {
    enum class Side { BUY, SELL };
    enum class Type { MARKET, LIMIT };

    uint64_t    id;
    uint64_t    timestamp;  // e.g., nanoseconds since epoch
    double      price;      // For LIMIT orders; ignored if Type::MARKET
    uint32_t    quantity;   // Remaining quantity
    Side        side;       // BUY or SELL
    Type        type;       // MARKET or LIMIT

    // Pointer to next in container (used by memory pool if needed)
    Order*      next_free;

    Order() = default;
    Order(uint64_t _id, uint64_t _timestamp, double _price, uint32_t _quantity, Side _side, Type _type)
        : id(_id), timestamp(_timestamp), price(_price), quantity(_quantity), side(_side), type(_type), next_free(nullptr) {}
};

#endif 
