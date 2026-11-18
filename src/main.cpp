#include "benchmark.hpp"
#include "workload.hpp"

#include <iostream>
#include <vector>


int main() {
   
    std::cout << "Low-latency pricing lab\n";


    WorkloadConfig config{1000, 1000};

    Workload workload = generateWorkload(config);
    const std::vector<BenchmarkSample> samples = benchmarkPricing1(workload, 5);
    const BenchmarkSummary summary = summarizeSamples(samples);


    std::cout << "Calculation count: "
          << workload.calculationCount()
          << '\n';

    // for (const BenchmarkSample& sample : samples) {
    //     std::cout << "Seconds:             " << sample.seconds << '\n';
    //     std::cout << "Calculations/second: "
    //             << sample.calculationsPerSecond << '\n';
    //     std::cout << "Checksum:            " << sample.checksum << '\n';
    // }

    std::cout << "Minimum seconds:       " << summary.minimumSeconds << '\n';
    std::cout << "Mean seconds:          " << summary.meanSeconds << '\n';
    std::cout << "Median seconds:        " << summary.medianSeconds << '\n';
    std::cout << "Maximum seconds:       " << summary.maximumSeconds << '\n';
    std::cout << "Median calculations/s: " << summary.medianCalculationsPerSecond << '\n';
    std::cout << "Checksum:              " << summary.checksum << '\n';





    return 0;

}



