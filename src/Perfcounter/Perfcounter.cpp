#include "Perfcounter.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <map>

std::unordered_map<Perfcounter::PerfType, Perfcounter::CounterStat> Perfcounter::_counters;

Perfcounter::Counter::Counter(Perfcounter::PerfType type):
    _type(type)
{
    Perfcounter::getCounter(type);
    _start = std::chrono::high_resolution_clock::now();
}

Perfcounter::Counter::~Counter()
{
    auto now = std::chrono::high_resolution_clock::now();
    Perfcounter::getCounter(_type).addCount(std::chrono::duration_cast<std::chrono::microseconds>(now - _start).count()); 
}

Perfcounter::CounterStat::CounterStat()
{
}

void Perfcounter::CounterStat::addCount(std::size_t count)
{
    mean_count_add += count;
    mean_count_num++;
    max_count = std::max(max_count, count);
    min_count = std::min(min_count, count);
}

std::double_t Perfcounter::CounterStat::getMeanCount() const
{
    return static_cast<std::double_t>(mean_count_add) / static_cast<std::double_t>(mean_count_num);
}

std::size_t Perfcounter::CounterStat::getMaxCount() const
{
    return max_count;
}

std::size_t Perfcounter::CounterStat::getMinCount() const
{
    return min_count;
}

std::size_t Perfcounter::CounterStat::getNumSample() const
{
    return mean_count_num;
}

Perfcounter::CounterStat &Perfcounter::getCounter(Perfcounter::PerfType type)
{
    return Perfcounter::_counters[type];
}

static std::string getPerfTypeString(Perfcounter::PerfType type)
{
    switch (type) {
        case Perfcounter::PerfType::BITSHIFT:
            return "BITSHIFT";
        case Perfcounter::PerfType::BITSHIFT2:
            return "BITSHIFT2";
        case Perfcounter::PerfType::EVALUATE_BOARD:
            return "EVALUATE_BOARD";
        case Perfcounter::PerfType::TIME_ALGO_FULL:
            return "TIME_ALGO_FULL";
        default:
            return "UNKNOWN-" + std::to_string(static_cast<std::size_t>(type));
    }
}

void Perfcounter::writeStats(const std::string &filename)
{
    std::ofstream file(filename);

    if (!file.good() || file.bad() || !file.is_open()) {
        return;
    }
    for (const auto &it : Perfcounter::_counters) {
        file << "Perfcounter::" << getPerfTypeString(it.first) << ":" << std::endl;
        file << "--- Mean count: " << static_cast<std::size_t>(it.second.getMeanCount()) << std::endl;
        file << "--- Max count: " << it.second.getMaxCount() << std::endl;
        file << "--- Min count: " << it.second.getMinCount() << std::endl;
        file << "--- Number of samples: " << it.second.getNumSample() << std::endl;
        file << std::endl;
    }
}
