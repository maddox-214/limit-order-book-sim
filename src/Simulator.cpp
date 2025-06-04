#include "Simulator.h"
#include "Benchmark.h"
#include <iostream>

Simulator::Simulator(OrderBook& ob, uint64_t seed)
    : book_(ob),
      rng_(seed),
      price_dist_(100.0, 200.0),      // e.g., prices between 100 and 200
      size_dist_(1, 1000),
      side_dist_(0.5),                // 50/50 buy vs. sell
      type_dist_(0.1),                // 10% market orders, 90% limit
      next_order_id_(1),
      next_timestamp_(1)
{
}

void Simulator::run(uint64_t total_orders)
{
    Benchmark timer;
    timer.start();

    for (uint64_t i = 0; i < total_orders; ++i) {
        double price = price_dist_(rng_);
        uint32_t qty = size_dist_(rng_);
        Order::Side side = side_dist_(rng_) ? Order::Side::BUY : Order::Side::SELL;
        Order::Type type = type_dist_(rng_) ? Order::Type::MARKET : Order::Type::LIMIT;

        // If MARKET, price can be dummy (0.0)
        double order_price = (type == Order::Type::LIMIT) ? price : 0.0;
        uint64_t ts = next_timestamp_++;

        book_.processOrder(next_order_id_++, ts, order_price, qty, side, type);
    }

    timer.stop();
    double elapsed_ms = timer.elapsedMilliseconds();
    double throughput = (double)total_orders / (elapsed_ms / 1000.0);

    std::cout << "Simulated " << total_orders << " orders in " << elapsed_ms << " ms\n";
    std::cout << "Throughput: " << throughput << " orders/sec\n";
}
