cmake_minimum_required(VERSION 3.20)
project(LowLatencyPricing)
set(CMAKE_CXX_STANDARD 17)
add_executable(benchmark main.cpp)
add_library(pricing_core
    workload.cpp
    pricing1.cpp
    benchmark.cpp
    pricing2.cpp
    normal_cdf.cpp
)
target_include_directories(pricing_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(benchmark PRIVATE pricing_core)

enable_testing()

add_executable(pricing_tests
    tests/pricing_tests.cpp
)

target_link_libraries(pricing_tests
    PRIVATE pricing_core
)

add_test(
    NAME pricing_tests
    COMMAND pricing_tests
)
