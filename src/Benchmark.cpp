#include "Benchmark.h"

void Benchmark::start()
{
    begin_ = std::chrono::high_resolution_clock::now();
}

void Benchmark::stop()
{
    end_ = std::chrono::high_resolution_clock::now();
}

double Benchmark::elapsedMilliseconds() const
{
    return std::chrono::duration<double, std::milli>(end_ - begin_).count();
}
