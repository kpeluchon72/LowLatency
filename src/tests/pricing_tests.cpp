#include <cmath>
#include <cstddef>
#include <iostream>
#include "option.hpp"
#include "pricing1.hpp"
#include <stdexcept>
#include "workload.hpp"
#include <vector>
#include "benchmark.hpp"
#include "pricing2.hpp"

int main() {
    constexpr double tolerance = 1e-9;
    constexpr double expectedCall = 6.594025324130;

    Option option{
        OptionType::Call,
        100.0,
        0.5,
        0.20,
        0.05,
        0.01
    };

    const double actualCall = pricing1::price(100.0, option);

    if (std::abs(actualCall - expectedCall) > tolerance) {
        std::cerr << "Call test failed: expected "
                  << expectedCall
                  << ", got "
                  << actualCall
                  << '\n';

        return 1;
    }

    constexpr double expectedPut = 4.623768607695;
    option.type = OptionType::Put;

    const double actualPut = pricing1::price(100.0, option);

    if (std::abs(actualPut - expectedPut) > tolerance) {
         std::cerr << "Put test failed: expected "
                  << expectedPut
                  << ", got "
                  << actualPut
                  << '\n';

        return 1;
    }

    const double parityLeft = actualCall - actualPut;

    const double parityRight =
        100.0 * std::exp(-option.dividendYield * option.timeToExpiry)
        - option.strike
        * std::exp(-option.riskFreeRate * option.timeToExpiry);

    if (std::abs(parityLeft - parityRight) > tolerance) {
        std::cerr << "Parity test failed: Left "
                  << parityLeft
                  << ", Right "
                  << parityRight
                  << '\n';
        return 1;
    }


    bool caughtInvalidConfig {false};

    try {
        WorkloadConfig config{0, 1};
        config.validate();
    } 
    catch (const std::invalid_argument&) {
        caughtInvalidConfig  = true;
    }

    if (!caughtInvalidConfig) {
        std::cerr << "Expected zero option count to be rejected\n";
        return 1;
    }

    WorkloadConfig config{2, 3};
    Workload workload = generateWorkload(config);
    std::vector<double> calculatedPrices(workload.calculationCount());
    pricing1::priceWorkload(workload, calculatedPrices);
    
    if (calculatedPrices.size() != workload.calculationCount()) {
        std::cerr << "Batch size mismatch: expected "
                << workload.calculationCount()
                << ", got "
                << calculatedPrices.size()
                << '\n';

        return 1;
    }

    for (double calculatedPrice : calculatedPrices) {
        if (!std::isfinite(calculatedPrice)
            || calculatedPrice < -tolerance)
        {
            std::cerr << "Batch produced an invalid price: "
                    << calculatedPrice
                    << '\n';

            return 1;
        }
    }

    const std::vector<pricing2::PreparedOption> preparedOptions =
        pricing2::prepareOptions(workload.options);

    std::vector<double> pricing2Prices(
        workload.calculationCount());

    pricing2::priceWorkload(
        workload,
        preparedOptions,
        pricing2Prices);

    if (pricing2Prices.size() != calculatedPrices.size()) {
        std::cerr << "Pricing1/Pricing2 batch size mismatch: "
                  << calculatedPrices.size()
                  << " versus "
                  << pricing2Prices.size()
                  << '\n';
        return 1;
    }

    double maximumAbsoluteError{0.0};

    for (std::size_t index{0};
         index < calculatedPrices.size();
         ++index)
    {
        const double error =
            std::abs(calculatedPrices[index] - pricing2Prices[index]);

        if (error > maximumAbsoluteError) {
            maximumAbsoluteError = error;
        }

        if (!std::isfinite(pricing2Prices[index])
            || pricing2Prices[index] < -tolerance)
        {
            std::cerr << "Pricing2 produced invalid output at index "
                      << index
                      << ": "
                      << pricing2Prices[index]
                      << '\n';
            return 1;
        }
    }

    if (maximumAbsoluteError > tolerance) {
        std::cerr << "Pricing1/Pricing2 maximum error: "
                  << maximumAbsoluteError
                  << '\n';
        return 1;
    }


    const std::vector<BenchmarkSample> syntheticSamples{
        {3.0, 42.0, 30.0},
        {1.0, 42.0, 10.0},
        {2.0, 42.0, 20.0}
    };

    const BenchmarkSummary syntheticSummary1 = summarizeSamples(syntheticSamples);

    if (std::abs(syntheticSummary1.minimumSeconds - 1.0)
    > tolerance)
    {
        std::cerr << "Summary minimum test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary1.meanSeconds - 2.0)
        > tolerance)
    {
        std::cerr << "Summary mean test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary1.medianSeconds - 2.0)
        > tolerance)
    {
        std::cerr << "Summary median test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary1.maximumSeconds - 3.0)
        > tolerance)
    {
        std::cerr << "Summary maximum test failed\n";
        return 1;
    }

    if (std::abs(
            syntheticSummary1.medianCalculationsPerSecond - 20.0)
        > tolerance)
    {
        std::cerr << "Summary median throughput test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary1.checksum - 42.0)
        > tolerance)
    {
        std::cerr << "Summary checksum test failed\n";
        return 1;
    }

    const std::vector<BenchmarkSample> evenSamples{
        {4.0, 42.0, 40.0},
        {1.0, 42.0, 10.0},
        {3.0, 42.0, 30.0},
        {2.0, 42.0, 20.0}
    };
    const BenchmarkSummary syntheticSummary2 = summarizeSamples(evenSamples);

    if (std::abs(syntheticSummary2.minimumSeconds - 1.0)
    > tolerance)
    {
        std::cerr << "Summary minimum test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary2.meanSeconds - 2.5)
        > tolerance)
    {
        std::cerr << "Summary mean test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary2.medianSeconds - 2.5)
        > tolerance)
    {
        std::cerr << "Summary median test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary2.maximumSeconds - 4.0)
        > tolerance)
    {
        std::cerr << "Summary maximum test failed\n";
        return 1;
    }

    if (std::abs(
            syntheticSummary2.medianCalculationsPerSecond - 25.0)
        > tolerance)
    {
        std::cerr << "Summary median throughput test failed\n";
        return 1;
    }

    if (std::abs(syntheticSummary2.checksum - 42.0)
        > tolerance)
    {
        std::cerr << "Summary checksum test failed\n";
        return 1;
    }


    option.type = OptionType::Call;

    const pricing2::PreparedOption preparedCall =
        pricing2::prepare(option);

    const double pricing2Call =
        pricing2::price(100.0, preparedCall);

    if (std::abs(actualCall - pricing2Call) > tolerance) {
        std::cerr << "Pricing 2 call failed. Actualcall: " << actualCall << " Pricing 2 Call: " << pricing2Call << '\n';
        return 1;
    }

    option.type = OptionType::Put;

    const pricing2::PreparedOption preparedPut =
        pricing2::prepare(option);

    const double pricing2Put =
        pricing2::price(100.0, preparedPut);
    
    if (std::abs(actualPut - pricing2Put) > tolerance) {
        std::cerr << "Pricing 2 put failed. ActualPut: " << actualPut << " Pricing 2 Put: " << pricing2Put << '\n';
        return 1;
    }



    return 0;
}
