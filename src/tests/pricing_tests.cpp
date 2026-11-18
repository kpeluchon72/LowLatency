#include <cmath>
#include <iostream>
#include "option.hpp"
#include "pricing1.hpp"
#include <stdexcept>
#include "workload.hpp"
#include <vector>

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



    return 0;
}
