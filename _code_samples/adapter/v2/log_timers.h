/*
 * Toy example: log timers
 */

#pragma once

#include <chrono>
#include <ostream>

#include "timers.h"

namespace example {

/*
 * Naive CSV backend (we don't enforce fields order)
 */
class CSV_writter
{
public:
    CSV_writter() = default;

    CSV_writter(std::ostream& out) : output_(out) {}

    void startRecord()
    {
        sep = false;
    }
    void endRecord()
    {
        output_ << std::endl;
    }

    template <typename Value>
    void writeKeyPair(const std::string& fieldName, const Value& value);

protected:
    /*
     * destructor can't be called on pointer of type CSV_writter
     *
     * This eliminate the risk of deleting a pointer of derived class with the wrong type
     * without adding the cost of virtual overhead.
     *
     * Note that there's no reason to use that class directly.
     */
    ~CSV_writter() = default;

private:
    // output stream default to std::cout
    std::ostream& output_ = std::cout;
    bool sep = false;
};

template <typename Value>
void CSV_writter::writeKeyPair(const std::string&, const Value& value)
{
    if (sep) {
        output_ << ",";
    }
    output_ << value;
    sep = true;
}

template <>
void CSV_writter::writeKeyPair<std::string>(const std::string&, const std::string& value)
{
    if (sep) {
        output_ << ",";
    }
    // naive string escaping, this is just an example
    output_ << '"' << value << '"';
    sep = true;
}

template <typename Key_Pair_Formatter>
class Abstract_log_timers : protected Key_Pair_Formatter
{
public:
    Abstract_log_timers()  = default;
    ~Abstract_log_timers() = default;

    Abstract_log_timers(std::ostream& out) : output_(out), Key_Pair_Formatter(out) {}

    /*
     * Log a set of timers with a name
     */
    void operator()(const std::string& name, const Timers& timers);

private:
    // output stream default to std::cout
    std::ostream& output_ = std::cout;
};

template<typename Key_Pair_Formatter>
inline void Abstract_log_timers<Key_Pair_Formatter>::operator()(const std::string& name, const Timers& timers)
{
    this->startRecord();
    this->writeKeyPair("name", name);
    this->writeKeyPair("init", timers.init.count());
    this->writeKeyPair("comput", timers.comput.count());
    this->writeKeyPair("postprocess", timers.postprocess.count());
    this->endRecord();
}

/*
 * Provide same behaviour as previous class
 */
using log_timers = Abstract_log_timers<CSV_writter>;

}
