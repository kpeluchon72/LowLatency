#include "normal_cdf.hpp"

#include <cmath>

double normalCDF::reference(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

