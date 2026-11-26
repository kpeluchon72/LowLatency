#include "cdf_benchmark.hpp"

std::vector<double> generateCDFInputs(std::size_t count, double minimum, double maximum) {

    // cant just have one value in range of test inputs

    if (count < 2){
        throw std::invalid_argument("Count cant be less than 2");
    }

    if (minimum >= maximum) {
        throw std::invalid_argument("min can't be greater than max");
    }

    std::vector<double> testInputs;
    testInputs.reserve(count);

    const double step = (maximum - minimum) / static_cast<double>(count - 1);

    for (std::size_t index{0}; index < count; ++index) {
        testInputs.push_back(minimum + static_cast<double>(index) * step);
    }

    return testInputs;

}
