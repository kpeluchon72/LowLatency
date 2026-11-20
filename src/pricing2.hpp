#pragma once

#include "option.hpp"
#include <vector>
#include "workload.hpp"

namespace pricing2 {
    struct PreparedOption {
        OptionType type;
        double logStrike;
        double sigmaSqrtTime;
        double inverseSigmaSqrtTime;
        double driftTerm;
        double dividendDiscount;
        double discountedStrike;
    };

    PreparedOption prepare(const Option& option);

    double price(double spot, const PreparedOption& option);

    std::vector<PreparedOption> prepareOptions(const std::vector<Option>& options);

    void priceWorkload(const Workload& workload, const std::vector<PreparedOption>& preparedOptions, std::vector<double>& output);

}
