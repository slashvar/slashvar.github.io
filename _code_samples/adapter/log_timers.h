/*
 * Toy example: log timers
 */

#pragma once

#include <chrono>
#include <ostream>

#include "timers.h"

namespace example {

class log_timers
{
public:
    log_timers()  = default;
    ~log_timers() = default;

    log_timers(std::ostream& out) : output_(out) {}

    /*
     * Log a set of timers with a name
     */
    void operator()(const std::string& name, const Timers& timers);

private:
    // output stream default to std::cout
    std::ostream& output_ = std::cout;
};

inline void log_timers::operator()(const std::string& name, const Timers& timers)
{
    // Hardcoded CSV format
    // Assume no string ambiguity
    output_ << '"' << name << "\"," << timers.init.count() << "," << timers.comput.count()
            << "," << timers.postprocess.count() << std::endl;
}

}
