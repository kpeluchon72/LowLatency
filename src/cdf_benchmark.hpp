#pragma once

#include <cstddef>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <cmath>


struct CDFBenchmarkSample {
    double seconds;
    double checksum;
    double evaluationsPerSecond;
};

struct CDFBenchmarkSummary {
    double minimumSeconds;
    double meanSeconds;
    double medianSeconds;
    double maximumSeconds;
    double medianEvaluationsPerSecond;
    double nanosecondsPerEvaluation;
    double checksum;
};

struct CDFAccuracySummary {
    double maximumAbsoluteError;
    double meanAbsoluteError;
    double rootMeanSquaredError;
    double inputAtMaximumError;
    double maximumSymmetryError;
    bool outputsInRange;
    bool monotonic;
};


template <auto CandidateFunction, auto ReferenceFunction> 
CDFAccuracySummary measureCDFAccuracy(const std::vector<double>& inputs) {

    if (inputs.empty()) {
        throw std::invalid_argument("CDF accuracy inputs cannot be empty");
    }

    double maximumAbsoluteError{0.0};
    double absoluteErrorSum{0.0};
    double squaredErrorSum{0.0};
    double inputAtMaximumError{inputs.front()};
    double maximumSymmetryError{0.0};

    bool outputsInRange{true};
    bool monotonic{true};
    bool firstOutput{true};

    double previousActual{0.0};

    for (double x : inputs) {
        const double expected = ReferenceFunction(x);
        const double actual = CandidateFunction(x);

        if (!std::isfinite(actual)) {
            outputsInRange = false;
            monotonic = false;
        }

        const double error = std::abs(actual - expected);

        absoluteErrorSum += error;
        squaredErrorSum += error * error;

    
        if (error > maximumAbsoluteError) {
            maximumAbsoluteError = error;
            inputAtMaximumError = x;
        }

        if (actual < 0.0 || actual > 1.0) {
            outputsInRange = false;
        }

        if (!firstOutput && actual < previousActual) {
            monotonic = false;
        }

        const double symmetryError = std::abs(CandidateFunction(x) + CandidateFunction(-x) - 1.0);

        if (symmetryError > maximumSymmetryError) {
            maximumSymmetryError = symmetryError;
        }

        previousActual = actual;
        firstOutput = false;

    }

    const double inputCount = static_cast<double>(inputs.size());
    const double meanAbsoluteError = absoluteErrorSum / inputCount;
    const double rootMeanSquaredError = std::sqrt(squaredErrorSum / inputCount);

    return CDFAccuracySummary{
        maximumAbsoluteError,
        meanAbsoluteError,
        rootMeanSquaredError,
        inputAtMaximumError,
        maximumSymmetryError,
        outputsInRange,
        monotonic
    };
    
}



CDFBenchmarkSummary summarizeCDFSamples(const std::vector<CDFBenchmarkSample>& samples);

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

    