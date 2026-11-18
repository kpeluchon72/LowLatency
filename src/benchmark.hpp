#pragma once

#include "workload.hpp"
#include <cstddef>
#include <vector>

struct BenchmarkSample {
    double seconds;
    double checksum;
    double calculationsPerSecond;
};

std::vector<BenchmarkSample> benchmarkPricing1(const Workload& workload, std::size_t timedRuns);

struct BenchmarkSummary {
    double minimumSeconds;
    double meanSeconds;
    double medianSeconds;
    double maximumSeconds;
    double medianCalculationsPerSecond;
    double checksum;
};

BenchmarkSummary summarizeSamples(const std::vector<BenchmarkSample>& samples);