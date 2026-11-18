#include "workload.hpp"
#include <cmath> 
#include <stdexcept>

std::size_t Workload::calculationCount() const {
    return options.size() * spotPrices.size();
}

void Workload::validate() const {

    // Check for empty options vector
    if (options.empty()) {
        throw std::invalid_argument("Workload must contain at least one option");
    }

    // Check for empty spotPrices Vector
    if (spotPrices.empty()) {
        throw std::invalid_argument("Spot prices cannot be empty");
    }

    // Check for non finite and non positive spot prices
    for (double spot : spotPrices) {
        if (!std::isfinite(spot) || spot <= 0.0 ) {
            throw std::invalid_argument("Spot prices values invalid");
        } 
    }

    // Check for non finite and non positive strike prices, time to expiry, and volatility
    for (const Option& option : options) {
        if (!std::isfinite(option.strike) || option.strike <= 0.0) {
            throw std::invalid_argument("Strike prices are invalid");
        }

        if (!std::isfinite(option.timeToExpiry) || option.timeToExpiry <= 0.0) {
            throw std::invalid_argument("Times to expiry are invalid");
        }

        if (!std::isfinite(option.volatility) || option.volatility <= 0.0) {
            throw std::invalid_argument("Volatility values are invalid");
        }

        if (!std::isfinite(option.riskFreeRate)) {
            throw std::invalid_argument("Risk free rate values are invalid");
        }

        if (!std::isfinite(option.dividendYield)) {
            throw std::invalid_argument("Dividend Yield values are invalid");
        }

    }
}


void WorkloadConfig::validate() const {
    if (optionCount == 0 || spotCount == 0) {
        throw std::invalid_argument("Option and spot counts must be positive");
    }
}


Workload generateWorkload(const WorkloadConfig& config) {
    config.validate();
    
    Workload newWorkload{};

    newWorkload.options.reserve(config.optionCount);
    newWorkload.spotPrices.reserve(config.spotCount);
    
    double fraction{0.0};
    double strike{0.0};
    OptionType type;
    

    for (std::size_t i{0}; i < config.optionCount; i++) {
        if (config.optionCount > 1) {
            fraction =
                static_cast<double>(i) /
                static_cast<double>(config.optionCount - 1);
        }

        strike = 80.0 +(120.0 - 80.0) * fraction;

        if (i % 2 == 0) {
            type = OptionType::Call;
        } else {
            type = OptionType::Put;
        }

        Option option{type, strike, 1, 0.20, 0.05, 0.01};
        
        newWorkload.options.push_back(option);
    }

    constexpr double twoPi = 6.283185307179586;
    double angle{0.0};
    double spot{0.0};

    for (std::size_t i{0}; i < config.spotCount; ++i) {
        angle = twoPi * static_cast<double>(i) / static_cast<double>(config.spotCount);

        spot = 100 + 10* std::sin(angle);

        newWorkload.spotPrices.push_back(spot);
    }

    newWorkload.validate();

    return newWorkload;
}