#pragma once

#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <map>
#include <cmath>
#include <string>
#include <unordered_map>

class Perfcounter {
public:
    enum class PerfType : std::size_t {
        BITSHIFT = 0,
        BITSHIFT2 = 1,
        EVALUATE_BOARD,
        TIME_ALGO_FULL,
    };

    class Counter {
    public:
        Counter(PerfType type);
        ~Counter();

    private:
        std::chrono::high_resolution_clock::time_point _start;
        PerfType _type;
    };

    class CounterStat {
    public:
        CounterStat();

        void addCount(std::size_t count);
        std::double_t getMeanCount() const;
        std::size_t getMaxCount() const;
        std::size_t getMinCount() const;
        std::size_t getNumSample() const;

    private:
        std::size_t mean_count_add = 0;
        std::size_t mean_count_num = 0;
        std::size_t max_count = 0;
        std::size_t min_count = UINT_MAX;
    };

    static CounterStat &getCounter(PerfType type);

    static void writeStats(const std::string &filename);

private:
    Perfcounter();

    static std::unordered_map<PerfType, CounterStat> _counters;
};
