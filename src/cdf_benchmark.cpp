#include "cdf_benchmark.hpp"
#include <algorithm>


std::vector<double> generateCDFInputs(std::size_t count, double minimum, double maximum) {

    // cant just have one value in range of test inputs

    if (count < 2){
        throw std::invalid_argument("Count cant be less than 2");
    }

    if (minimum >= maximum) {
        throw std::invalid_argument("min must be less than max");
    }

    std::vector<double> testInputs;
    testInputs.reserve(count);

    const double step = (maximum - minimum) / static_cast<double>(count - 1);

    for (std::size_t index{0}; index < count; ++index) {
        testInputs.push_back(minimum + static_cast<double>(index) * step);
    }

    return testInputs;

}


CDFBenchmarkSummary summarizeCDFSamples(const std::vector<CDFBenchmarkSample>& samples) {
    if (samples.empty()) {
        throw std::invalid_argument(
            "Cannot summarize empty benchmark samples");
    }

    std::vector<double> seconds;
    std::vector<double> evaluationsPerSecond;

    seconds.reserve(samples.size());
    evaluationsPerSecond.reserve(samples.size());

    double secondsSum{0.0};
    const double expectedChecksum = samples.front().checksum;


    for (const CDFBenchmarkSample& sample : samples) {
        seconds.push_back(sample.seconds);
        evaluationsPerSecond.push_back(sample.evaluationsPerSecond);
        secondsSum += sample.seconds;

        if (sample.checksum != expectedChecksum) {
            throw std::runtime_error(
                "Benchmark checksums do not match");
        }
    }

    std::sort(seconds.begin(), seconds.end());
    std::sort(evaluationsPerSecond.begin(), evaluationsPerSecond.end());

    const double meanSeconds = secondsSum / static_cast<double>(samples.size());

    const std::size_t middle = samples.size() / 2;

    double medianSeconds{0.0};
    double medianEvaluationsPerSecond{0.0};   

    if (samples.size() % 2 == 0) {
        medianSeconds =
            (seconds[middle - 1] + seconds[middle]) / 2.0;

        medianEvaluationsPerSecond =
            (evaluationsPerSecond[middle - 1] + evaluationsPerSecond[middle]) / 2.0;
    } else {
        medianSeconds = seconds[middle];
        medianEvaluationsPerSecond = evaluationsPerSecond[middle];
    }

    const double nanosecondsPerEvaluation = 1'000'000'000.0 / medianEvaluationsPerSecond;

    return CDFBenchmarkSummary{
        seconds.front(),
        meanSeconds,
        medianSeconds,
        seconds.back(),
        medianEvaluationsPerSecond,
        nanosecondsPerEvaluation,
        expectedChecksum
    };
}