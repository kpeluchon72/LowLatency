#pragma once

#include <vector>
#include "option.hpp"
#include <cstddef>

struct WorkloadConfig {
    std::size_t optionCount;
    std::size_t spotCount;

    void validate() const; 
};


struct Workload {
    std::vector<Option> options;
    std::vector<double> spotPrices;

    std::size_t calculationCount() const;
    void validate() const;
};

Workload generateWorkload(const WorkloadConfig& config);