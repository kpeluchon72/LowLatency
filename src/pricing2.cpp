#include "pricing2.hpp"
#include "pricing1.hpp"
#include <cmath>

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
        double call = (spot * option.dividendDiscount * pricing1::normalCDF(d1)) - (option.discountedStrike * pricing1::normalCDF(d2));
        return call;
    } else {
        double put = (option.discountedStrike * pricing1::normalCDF(-d2)) - (spot * option.dividendDiscount * pricing1::normalCDF(-d1));
        return put;
    }
}
