#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "OrderBook.h"
#include <random>
#include <cstdint>

// Generates random order flows (market + limit) and feeds them into the OrderBook
// Simulates N total orders at a target arrival rate.
class Simulator {
public:
    explicit Simulator(OrderBook& ob, uint64_t seed = 42);

    // Run simulation for total_orders; returns vector of all fills (optional).
    void run(uint64_t total_orders);

private:
    OrderBook&                book_;
    std::mt19937_64           rng_;
    std::uniform_real_distribution<double>  price_dist_;
    std::uniform_int_distribution<uint32_t> size_dist_;
    std::bernoulli_distribution            side_dist_;
    std::bernoulli_distribution            type_dist_;

    uint64_t                  next_order_id_;
    uint64_t                  next_timestamp_;  // simple incrementing counter
};

#endif 
