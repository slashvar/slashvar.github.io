#include <chrono>
#include <iostream>
#include <string>
#include <unistd.h>

namespace Logger {
template<typename Backend>
struct base : public Backend
{
    void print(const std::string& msg)
    {
        auto ts = std::chrono::system_clock::now().time_since_epoch().count();
        auto out = std::to_string(ts) + ":" + std::to_string(getpid()) + ":" + msg;
        this->write(out);
    }
};

struct std_err_backend
{
    void write(const std::string& str)
    {
        std::clog << str << std::endl;
    }
};

using ErrOut = base<std_err_backend>;

};

int main()
{
    Logger::ErrOut log;
    log.print("a message");
}
