/*
 * Toy example: a collection of timers
 */

#pragma once

#include <chrono>

namespace example {
using std::chrono::seconds;
using namespace std::chrono_literals;

/*
 * A simple POD storing a set of duration in seconds
 */
struct Timers
{
    seconds init        = 0s;
    seconds comput      = 0s;
    seconds postprocess = 0s;
};
}
