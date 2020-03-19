/*
 * Toy example: testing
 */

#include <chrono>
#include <iostream>

#include "log_timers.h"
#include "timers.h"

using namespace std::chrono_literals;

inline example::Timers produce_sample()
{
    return { 1s, 5s, 2s };
}

inline std::string sample_name(const unsigned i)
{
    return "sample" + std::to_string(i);
}

inline std::pair<std::string, example::Timers> new_sample(const unsigned i)
{
    return { sample_name(i), produce_sample() };
}

inline void demo(const unsigned amount)
{
    example::log_timers log;
    for (unsigned i = 0; i < amount; ++i) {
        auto [name, timers] = new_sample(i);
        log(name, timers);
    }
}

int main()
{
    demo(5);
}
