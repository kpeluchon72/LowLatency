#include "normal_cdf.hpp"

#include <cmath>

double normalCDF::reference(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double normalCDF::abramowitzStegun(double x) {
    constexpr double p = 0.2316419;
    constexpr double b1 = 0.319381530;
    constexpr double b2 = -0.356563782;
    constexpr double b3 = 1.781477937;
    constexpr double b4 = -1.821255978;
    constexpr double b5 = 1.330274429;
    constexpr double inverseSqrtTwoPi = 0.39894228040143267794;

    const double z = std::abs(x);
    const double t = 1.0 / (1.0 + p * z);

    const double density = inverseSqrtTwoPi * std::exp(-0.5 * z * z);
    const double polynomial = (((((b5*t + b4) * t + b3) * t + b2) * t + b1) * t);

    const double positiveCDF = 1.0 - density * polynomial;

    return x >= 0.0 ? positiveCDF : 1.0 - positiveCDF;

}


