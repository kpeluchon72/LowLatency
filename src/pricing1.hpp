#pragma once

#include "option.hpp"
#include <vector>
#include "workload.hpp"


namespace pricing1 {
    double price(double spot, const Option& option);
    void priceWorkload(
        const Workload& workload,
        std::vector<double>& output);
}
