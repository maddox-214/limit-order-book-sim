#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>

// Simple timer to measure elapsed time in milliseconds
class Benchmark {
public:
    void start();
    void stop();
    double elapsedMilliseconds() const;

private:
    std::chrono::high_resolution_clock::time_point begin_;
    std::chrono::high_resolution_clock::time_point end_;
};
#endif 
