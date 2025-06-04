#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <cstdint>
#include <map>
#include <deque>
#include <vector>
#include <unordered_map>    
#include "Order.h"
#include "MemoryPool.h"

// The matching engine: maintains two order books (bids & asks) and matches incoming orders.
// Uses price‐time priority.
class OrderBook {
public:
    explicit OrderBook(size_t max_orders);

    // When an order is processed, any fills are returned in this struct:
    struct Fill {
        uint64_t resting_id;    // the order already in book
        uint64_t incoming_id;   // the new order
        uint32_t quantity;      // how many shares/contracts traded
        double price;           // at what price
    };

    // Process a new order; returns a vector of Fill events.
    // id, timestamp, price, quantity, side, type
    std::vector<Fill> processOrder(uint64_t id,
                                   uint64_t timestamp,
                                   double price,
                                   uint32_t quantity,
                                   Order::Side side,
                                   Order::Type type);

    // Cancel an existing order by its ID. Returns true if found+cancelled.
    bool cancelOrder(uint64_t id);

    // Diagnostics: how many orders are active in the book?
    size_t totalActiveOrders() const;

private:
    MemoryPool                                      pool_;      // preallocated Orders
    std::map<double, std::deque<Order*>,            // bids: price -> queue (highest price first)
             std::greater<double>> bids_;
    std::map<double, std::deque<Order*>>            asks_;      // asks: price -> queue (lowest price first)

    // Map from order ID >> Order*, so we can cancel in O(1)
    std::unordered_map<uint64_t, Order*>            id_map_;

    size_t                                          max_orders_;
    size_t                                          active_orders_;

    // Match a limit order against the opposite side
    void matchLimitOrder(Order* incoming, std::vector<Fill>& fills);

    // Match a market order against the opposite side
    void matchMarketOrder(Order* incoming, std::vector<Fill>& fills);

    // If a limit order remains after matching, add it into the book
    void addToBook(Order* order);
};

#endif 
