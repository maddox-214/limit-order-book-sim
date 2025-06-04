
#include "OrderBook.h"
#include <iostream>  


// Constructor: initialize memory pool and counters
OrderBook::OrderBook(size_t max_orders)
    : pool_(max_orders),
      max_orders_(max_orders),
      active_orders_(0)
{
}


// processOrder: allocate a new Order, match it, and (if it's a LIMIT and still has qty) add to book.
// Returns a vector of Fill events.
std::vector<OrderBook::Fill> OrderBook::processOrder(
    uint64_t id,
    uint64_t timestamp,
    double price,
    uint32_t quantity,
    Order::Side side,
    Order::Type type)
{
    std::vector<Fill> fills;

    // 1) Allocate a slot from the pool
    Order* incoming = pool_.allocate();
    if (!incoming) {
        // Pool exhausted → return empty fills
        return fills;
    }

    // 2) Placement-new to initialize fields
    new (incoming) Order(id, timestamp, price, quantity, side, type);

    // 3) Match logic
    if (type == Order::Type::MARKET) {
        matchMarketOrder(incoming, fills);
    } else {
        matchLimitOrder(incoming, fills);
    }

    // 4) If it's a LIMIT and still has remaining quantity, rest in the book
    if (incoming->type == Order::Type::LIMIT && incoming->quantity > 0) {
        addToBook(incoming);
        id_map_[incoming->id] = incoming;
        ++active_orders_;
    } else {
        // Fully filled (or MARKET order) >> return slot to pool
        pool_.deallocate(incoming);
    }

    return fills;
}


// cancelOrder: find the resting order in the appropriate book and remove it.
// Returns true if the order ID was found & cancelled.
bool OrderBook::cancelOrder(uint64_t id)
{
    auto it = id_map_.find(id);
    if (it == id_map_.end()) return false;

    Order* toCancel = it->second;
    double price = toCancel->price;
    Order::Side side = toCancel->side;

    // Because bids_ uses a different comparator type than asks_,
    // we must handle each side in its own branch.

    if (side == Order::Side::BUY) {
        // BUY orders reside in bids_
        auto mapIt = bids_.find(price);
        if (mapIt == bids_.end()) return false;

        auto& queueAtPrice = mapIt->second;
        for (auto qIt = queueAtPrice.begin(); qIt != queueAtPrice.end(); ++qIt) {
            if ((*qIt)->id == id) {
                queueAtPrice.erase(qIt);
                id_map_.erase(it);
                pool_.deallocate(toCancel);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    bids_.erase(mapIt);
                }
                return true;
            }
        }
        return false;

    } else {
        // SELL orders reside in asks_
        auto mapIt = asks_.find(price);
        if (mapIt == asks_.end()) return false;

        auto& queueAtPrice = mapIt->second;
        for (auto qIt = queueAtPrice.begin(); qIt != queueAtPrice.end(); ++qIt) {
            if ((*qIt)->id == id) {
                queueAtPrice.erase(qIt);
                id_map_.erase(it);
                pool_.deallocate(toCancel);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    asks_.erase(mapIt);
                }
                return true;
            }
        }
        return false;
    }
}


// totalActiveOrders: how many orders are currently resting in the book
size_t OrderBook::totalActiveOrders() const
{
    return active_orders_;
}


// matchLimitOrder: attempt to fill a LIMIT order against the opposite book.
// We handle BUY and SELL in separate branches so that bids_/asks_ remain different types.
void OrderBook::matchLimitOrder(Order* incoming, std::vector<Fill>& fills)
{
    if (incoming->side == Order::Side::BUY) {
        // BUY limit >> match against asks_ (ascending prices)
        while (incoming->quantity > 0 && !asks_.empty()) {
            auto bestIt = asks_.begin();
            double bestPrice = bestIt->first;

            // If buy price < best ask, no match
            if (incoming->price < bestPrice) {
                break;
            }

            auto& queueAtPrice = bestIt->second;
            Order* resting = queueAtPrice.front();

            uint32_t tradedQty = std::min(incoming->quantity, resting->quantity);
            double tradePrice = bestPrice;

            fills.push_back({ resting->id, incoming->id, tradedQty, tradePrice });

            incoming->quantity -= tradedQty;
            resting->quantity -= tradedQty;

            if (resting->quantity == 0) {
                id_map_.erase(resting->id);
                queueAtPrice.pop_front();
                pool_.deallocate(resting);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    asks_.erase(bestIt);
                }
            }
        }

    } else {
        // SELL limit >> match against bids_ (highest price first)
        while (incoming->quantity > 0 && !bids_.empty()) {
            auto bestIt = bids_.begin();
            double bestPrice = bestIt->first;

            // If sell price > best bid, no match
            if (incoming->price > bestPrice) {
                break;
            }

            auto& queueAtPrice = bestIt->second;
            Order* resting = queueAtPrice.front();

            uint32_t tradedQty = std::min(incoming->quantity, resting->quantity);
            double tradePrice = bestPrice;

            fills.push_back({ resting->id, incoming->id, tradedQty, tradePrice });

            incoming->quantity -= tradedQty;
            resting->quantity -= tradedQty;

            if (resting->quantity == 0) {
                id_map_.erase(resting->id);
                queueAtPrice.pop_front();
                pool_.deallocate(resting);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    bids_.erase(bestIt);
                }
            }
        }
    }
}


// matchMarketOrder: attempt to fill a MARKET order against the opposite book.
// Similar to matchLimitOrder but ignores price checks.
void OrderBook::matchMarketOrder(Order* incoming, std::vector<Fill>& fills)
{
    if (incoming->side == Order::Side::BUY) {
        // BUY market >> match against asks_
        while (incoming->quantity > 0 && !asks_.empty()) {
            auto bestIt = asks_.begin();
            double bestPrice = bestIt->first;
            auto& queueAtPrice = bestIt->second;
            Order* resting = queueAtPrice.front();

            uint32_t tradedQty = std::min(incoming->quantity, resting->quantity);
            double tradePrice = bestPrice;

            fills.push_back({ resting->id, incoming->id, tradedQty, tradePrice });

            incoming->quantity -= tradedQty;
            resting->quantity -= tradedQty;

            if (resting->quantity == 0) {
                id_map_.erase(resting->id);
                queueAtPrice.pop_front();
                pool_.deallocate(resting);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    asks_.erase(bestIt);
                }
            }
        }

    } else {
        // SELL market >> match against bids_
        while (incoming->quantity > 0 && !bids_.empty()) {
            auto bestIt = bids_.begin();
            double bestPrice = bestIt->first;
            auto& queueAtPrice = bestIt->second;
            Order* resting = queueAtPrice.front();

            uint32_t tradedQty = std::min(incoming->quantity, resting->quantity);
            double tradePrice = bestPrice;

            fills.push_back({ resting->id, incoming->id, tradedQty, tradePrice });

            incoming->quantity -= tradedQty;
            resting->quantity -= tradedQty;

            if (resting->quantity == 0) {
                id_map_.erase(resting->id);
                queueAtPrice.pop_front();
                pool_.deallocate(resting);
                --active_orders_;
                if (queueAtPrice.empty()) {
                    bids_.erase(bestIt);
                }
            }
        }
    }
}


// addToBook: insert a resting LIMIT order into the correct side (bids_ or asks_)
void OrderBook::addToBook(Order* order)
{
    if (order->side == Order::Side::BUY) {
        bids_[order->price].push_back(order);
    } else {
        asks_[order->price].push_back(order);
    }
}
