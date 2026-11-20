#include "benchmark.hpp"
#include "workload.hpp"

#include <iomanip>
#include <iostream>
#include <vector>


int main() {
   
    std::cout << "Low-latency pricing lab\n";


    WorkloadConfig config{1000, 1000};

    Workload workload = generateWorkload(config);

    const std::vector<BenchmarkSample> pricing1Samples = benchmarkPricing1(workload, 5);
    const std::vector<BenchmarkSample> pricing2Samples = benchmarkPricing2(workload, 5);

    const BenchmarkSummary pricing1Summary = summarizeSamples(pricing1Samples);
    const BenchmarkSummary pricing2Summary = summarizeSamples(pricing2Samples);

    const double speedup = pricing2Summary.medianCalculationsPerSecond / pricing1Summary.medianCalculationsPerSecond;

    std::cout << "Calculation count: "
              << workload.calculationCount()
              << '\n';

    displaySummary("Pricing 1 - reference", pricing1Summary);
    displaySummary("Pricing 2 - precomputed", pricing2Summary);

    std::cout << "\nPricing 2 speedup: "
              << std::fixed << std::setprecision(2)
              << speedup << "x\n";

    return 0;

}



