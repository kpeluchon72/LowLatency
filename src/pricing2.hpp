#pragma once

#include "option.hpp"

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

}
