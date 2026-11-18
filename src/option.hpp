#pragma once

enum class OptionType {
    Call,
    Put
};


struct Option {
    OptionType type;
    double strike;
    double timeToExpiry;
    double volatility;
    double riskFreeRate;
    double dividendYield;
};


