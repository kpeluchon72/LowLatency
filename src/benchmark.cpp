#include "benchmark.hpp"
#include "pricing1.hpp"

#include <chrono>
#include <vector>
#include <stdexcept>

std::vector<BenchmarkSample> benchmarkPricing1(const Workload& workload, std::size_t timedRuns) {
    workload.validate();

    if (timedRuns == 0) {
        throw std::invalid_argument("TimedRuns must be positive");
    }


    std::vector<double> prices(workload.calculationCount());

    // warmup
    pricing1::priceWorkload(workload, prices);

    std::vector<BenchmarkSample> samples;
    samples.reserve(timedRuns);



    
    for (std::size_t run{0}; run < timedRuns; ++run) {
        const auto start = std::chrono::steady_clock::now();
        
        pricing1::priceWorkload(workload, prices);

        const auto finish = std::chrono::steady_clock::now();
        
        const double seconds = std::chrono::duration<double>(finish - start).count();

        double checksum{0.0};
        for (double calculatedPrice : prices) {
            checksum += calculatedPrice;
        }

        const double calculationsPerSecond = static_cast<double>(workload.calculationCount()) / seconds;
        
        samples.push_back(BenchmarkSample{
            seconds,
            checksum,
            calculationsPerSecond
        });

    }


    return samples;


}
