#include "benchmark.hpp"
#include "cdf_benchmark.hpp"
#include "normal_cdf.hpp"
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

    const std::vector<double> cdfInputs =
        generateCDFInputs(1'000'000, -8.0, 8.0);

    const std::vector<CDFBenchmarkSample> referenceSamples =
        benchmarkCDF<normalCDF::reference>(cdfInputs, 10);

    const CDFBenchmarkSummary referenceBenchmark =
        summarizeCDFSamples(referenceSamples);

    const CDFAccuracySummary referenceAccuracy =
        measureCDFAccuracy<
            normalCDF::reference,
            normalCDF::reference>(cdfInputs);

    std::cout << "\nNormal CDF comparison\n"
              << "Inputs: " << cdfInputs.size()
              << " from -8 to 8\n\n";

    displayCDFComparisonHeader();

    displayCDFComparison(
        "std::erfc reference",
        referenceBenchmark,
        referenceAccuracy,
        referenceBenchmark.medianEvaluationsPerSecond);

    return 0;

}



