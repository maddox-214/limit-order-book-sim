# ⚡ Limit Order Book Simulator

A high-performance C++17 limit order book (LOB) simulator for benchmarking market and limit order execution strategies under synthetic high-frequency trading (HFT) conditions.

---

## 🚀 Features

- 📈 **Real-time Matching Engine**  
  Handles market and limit orders using price-time priority.

- ⚡ **100K+ Orders per Second**  
  Optimized using STL containers, preallocated memory pools, and minimal heap allocation.

- 🧪 **Robust Order Flow Simulation**  
  Simulates realistic order traffic across 1M+ orders with configurable intensity.

- 📊 **Benchmarking Support**  
  Measures throughput and latency for performance profiling.

- 🧠 **Clean Systems Design**  
  Modular codebase with separation of concerns: OrderBook, Simulator, MemoryPool.

---

## 🛠️ Technologies

- **Language**: C++17  
- **Build System**: CMake  
- **Standard Library**: STL (priority_queue, map, etc.)

---

## 🔧 Building and Running

### Requirements:
- CMake 3.10+
- C++17-compatible compiler (MSVC, g++, clang++)

### Build

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release

./LOB_Simulator.exe 1000000 2000000


📈 Sample Benchmark Output
Processing 1,000,000 limit orders...
Processing 2,000,000 market orders...
Total matched orders: 1,973,420
Average latency: 4.3 µs
Throughput: 112,000 orders/sec


