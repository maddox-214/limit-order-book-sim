#include "OrderBook.h"
#include "Simulator.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    // Parse command-line arguments: total orders to simulate, pool size, etc.
    uint64_t total_orders = 1'000'000;    // default 1 million
    size_t   pool_capacity = 2'000'000;   // ensure enough preallocated slots

    if (argc >= 2) {
        total_orders = std::stoull(argv[1]);
    }
    if (argc >= 3) {
        pool_capacity = std::stoull(argv[2]);
    }

    std::cout << "Initializing order book with pool capacity " << pool_capacity << "\n";
    OrderBook book(pool_capacity);
    Simulator sim(book, /*seed=*/ 12345);

    std::cout << "Running simulation for " << total_orders << " orders...\n";
    sim.run(total_orders);

    std::cout << "Final active orders in book: " << book.totalActiveOrders() << "\n";
    return 0;
}
