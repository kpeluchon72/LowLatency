#pragma once

#include <cstddef>
#include <vector>
#include <stdexcept>
#include <chrono>


struct CDFBenchmarkSample {
    double seconds;
    double checksum;
    double evaluationsPerSecond;
};

template <auto CDFFunction> std::vector<CDFBenchmarkSample> benchmarkCDF(
    const std::vector<double>& inputs,
    std::size_t timedRuns)
{
    // Validate inputs and timedRuns
    if (inputs.empty()) {
        throw std::invalid_argument("CDF Benchmark inputs cannot be empty");
    }

    if (timedRuns == 0) {
        throw std::invalid_argument("CDF timed runs must be positive");
    }

    // Create an output vector the same size as inputs
    std::vector<double> outputs(inputs.size());

    // Create and reserve the samples vector
    std::vector<CDFBenchmarkSample> samples;
    samples.reserve(timedRuns);

    // Warm up by evaluating every input once
    for (std::size_t i{0}; i < inputs.size(); ++i) {
        outputs[i] = CDFFunction(inputs[i]);

    }


    for (std::size_t run{0}; run < timedRuns; ++run) {
        const auto start = std::chrono::steady_clock::now();

        for (std::size_t index{0}; index < inputs.size(); ++index) {
            outputs[index] = CDFFunction(inputs[index]);
        }

        const auto end = std::chrono::steady_clock::now();


        const double seconds = std::chrono::duration<double>(end - start).count();
        const double evaluationsPerSecond = static_cast<double>(inputs.size()) / seconds;

        double checksum{0.0};
        for (double output: outputs) {
            checksum += output;
        }


        CDFBenchmarkSample sample{
            seconds, 
            checksum, 
            evaluationsPerSecond
        };
        
        samples.push_back(sample);

    }

    // Return samples

    return samples;

}


std::vector<double> generateCDFInputs(
    std::size_t count,
    double minimum,
    double maximum);

    