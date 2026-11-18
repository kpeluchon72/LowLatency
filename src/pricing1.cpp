#include "pricing1.hpp"
#include <cmath>

double pricing1::normalCDF(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}


double pricing1::price(double spot, const Option& option) {
    double SIGMA_SQRT = option.volatility * std::sqrt(option.timeToExpiry);
    double INV_SIGMA_SQRT = 1 / (SIGMA_SQRT);

    double d1 = (((std::log(spot) - std::log(option.strike))
                + (option.riskFreeRate - option.dividendYield + 0.5 * option.volatility * option.volatility)
                * option.timeToExpiry)) * INV_SIGMA_SQRT;
    
    double d2 = d1 - SIGMA_SQRT;

    if (option.type == OptionType::Call) {
        double call = spot * std::exp(-option.dividendYield * option.timeToExpiry) * normalCDF(d1)
                        - option.strike * std::exp(-option.riskFreeRate * option.timeToExpiry) * normalCDF(d2);

        return call;

    } else {
        double put = option.strike * std::exp(-1 * option.riskFreeRate * option.timeToExpiry) * normalCDF(-d2)
                        - spot * std::exp(-1 * option.dividendYield * option.timeToExpiry) * normalCDF(-d1);
        
        return put;
    }

}


void pricing1::priceWorkload(const Workload& workload, std::vector<double>& output) {
    workload.validate();
    output.resize(workload.calculationCount());

    std::size_t index{0};

    for (const Option& option : workload.options) {
        for (double spot : workload.spotPrices) {
            output[index] = pricing1::price(spot, option);
            ++index;
        }
    }
}
