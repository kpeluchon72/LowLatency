#include "benchmark.hpp"
#include "workload.hpp"

#include <iostream>
#include <vector>


int main() {
   
    std::cout << "Low-latency pricing lab\n";


    WorkloadConfig config{1000, 1000};

    Workload workload = generateWorkload(config);
    const std::vector<BenchmarkSample> samples = benchmarkPricing1(workload, 5);

    std::cout << "Calculation count: "
          << workload.calculationCount()
          << '\n';

    for (const BenchmarkSample& sample : samples) {
        std::cout << "Seconds:             " << sample.seconds << '\n';
        std::cout << "Calculations/second: "
                << sample.calculationsPerSecond << '\n';
        std::cout << "Checksum:            " << sample.checksum << '\n';
    }



    return 0;

}



