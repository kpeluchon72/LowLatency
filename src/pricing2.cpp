#include "pricing2.hpp"
#include "normal_cdf.hpp"


#include <cmath>
#include <cstddef>
#include <stdexcept>

pricing2::PreparedOption pricing2::prepare(const Option& option) {
    double logStrike = std::log(option.strike);
    double sigmaSqrtTime = option.volatility * std::sqrt(option.timeToExpiry);
    double inverseSigmaSqrtTime = 1 / sigmaSqrtTime;
    double driftTerm = (option.riskFreeRate - option.dividendYield + 0.5* option.volatility * option.volatility) * option.timeToExpiry;
    double dividendDiscount = std::exp(-option.dividendYield * option.timeToExpiry);
    double discountedStrike = option.strike * std::exp(-option.riskFreeRate * option.timeToExpiry);

    return PreparedOption{option.type, logStrike, sigmaSqrtTime, inverseSigmaSqrtTime, driftTerm, dividendDiscount, discountedStrike};
}

double pricing2::price(double spot, const PreparedOption& option) {
    double d1 = (std::log(spot) - option.logStrike + option.driftTerm) * option.inverseSigmaSqrtTime;
    double d2 = d1 - option.sigmaSqrtTime;
    
    if (option.type == OptionType::Call) {
        double call = (spot * option.dividendDiscount * normalCDF::reference(d1)) - (option.discountedStrike * normalCDF::reference(d2));
        return call;
    } else {
        double put = (option.discountedStrike * normalCDF::reference(-d2)) - (spot * option.dividendDiscount * normalCDF::reference(-d1));
        return put;
    }
}

std::vector<pricing2::PreparedOption> pricing2::prepareOptions(const std::vector<Option>& options) {
    std::vector<PreparedOption> preparedOptions{};
    preparedOptions.reserve(options.size());

    for (const Option& option : options) {
        preparedOptions.push_back(prepare(option));
    }

    return preparedOptions;
}


void pricing2::priceWorkload(
    const Workload& workload, 
    const std::vector<pricing2::PreparedOption>& preparedOptions, 
    std::vector<double>& output) {

    workload.validate();

    if (preparedOptions.size() != workload.options.size()) {
        throw std::invalid_argument(
            "Prepared option count does not match workload");
    }
    
    output.resize(workload.calculationCount());
        
    std::size_t index{0};

    for (const PreparedOption& preparedOption : preparedOptions) {
        for (double spot : workload.spotPrices) {
            output[index] = pricing2::price(spot, preparedOption);
            ++index;
        }
    }


}

