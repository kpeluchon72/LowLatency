#include "benchmark.hpp"
#include "pricing1.hpp"
#include "pricing2.hpp"

#include <chrono>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

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

std::vector<BenchmarkSample> benchmarkPricing2(const Workload& workload, std::size_t timedRuns) {
    workload.validate();

    if (timedRuns == 0) {
        throw std::invalid_argument("TimedRuns must be positive");
    }

    // Prepare options before hand - precompute
    const std::vector<pricing2::PreparedOption> preparedOptions = pricing2::prepareOptions(workload.options);

    std::vector<double> prices(workload.calculationCount());

    // warmup
    pricing2::priceWorkload(workload, preparedOptions, prices);

    std::vector<BenchmarkSample> samples;
    samples.reserve(timedRuns);

    
    for (std::size_t run{0}; run < timedRuns; ++run) {
        const auto start = std::chrono::steady_clock::now();
        
        pricing2::priceWorkload(workload, preparedOptions, prices);

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


BenchmarkSummary summarizeSamples(const std::vector<BenchmarkSample>& samples) {
    if (samples.empty()) {
        throw std::invalid_argument(
            "Cannot summarize empty benchmark samples");
    }
    
    std::vector<double> seconds;
    std::vector<double> throughputs;

    seconds.reserve(samples.size());
    throughputs.reserve(samples.size());

    double secondsSum{0.0};
    const double expectedChecksum = samples.front().checksum;


    for (const BenchmarkSample& sample : samples) {
        seconds.push_back(sample.seconds);
        throughputs.push_back(sample.calculationsPerSecond);
        secondsSum += sample.seconds;

        if (sample.checksum != expectedChecksum) {
            throw std::runtime_error(
                "Benchmark checksums do not match");
        }
    }

    std::sort(seconds.begin(), seconds.end());
    std::sort(throughputs.begin(), throughputs.end());

    const double meanSeconds = secondsSum / static_cast<double>(samples.size());

    const std::size_t middle = samples.size() / 2;

    double medianSeconds{0.0};
    double medianThroughput{0.0};   

    if (samples.size() % 2 == 0) {
        medianSeconds =
            (seconds[middle - 1] + seconds[middle]) / 2.0;

        medianThroughput =
            (throughputs[middle - 1] + throughputs[middle]) / 2.0;
    } else {
        medianSeconds = seconds[middle];
        medianThroughput = throughputs[middle];
    }

    return BenchmarkSummary{
        seconds.front(),
        meanSeconds,
        medianSeconds,
        seconds.back(),
        medianThroughput,
        expectedChecksum
    };

}


void displaySummary(
    std::string_view name,
    const BenchmarkSummary& summary)
{
    constexpr int labelWidth = 28;

    std::cout << "\n=== " << name << " ===\n"
              << std::fixed << std::setprecision(6)
              << std::left << std::setw(labelWidth)
              << "Minimum time:" << std::right
              << summary.minimumSeconds << " s\n"
              << std::left << std::setw(labelWidth)
              << "Mean time:" << std::right
              << summary.meanSeconds << " s\n"
              << std::left << std::setw(labelWidth)
              << "Median time:" << std::right
              << summary.medianSeconds << " s\n"
              << std::left << std::setw(labelWidth)
              << "Maximum time:" << std::right
              << summary.maximumSeconds << " s\n"
              << std::left << std::setw(labelWidth)
              << "Median throughput:" << std::right
              << std::setprecision(2)
              << summary.medianCalculationsPerSecond / 1'000'000.0
              << " M prices/s\n"
              << std::left << std::setw(labelWidth)
              << "Checksum:" << std::right
              << std::scientific << std::setprecision(6)
              << summary.checksum << '\n'
              << std::defaultfloat;
}

