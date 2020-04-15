#include <chrono>
#include <iostream>
#include <vector>

template <typename Duration>
struct time_guard
{
    using clock = std::chrono::steady_clock;

    time_guard(Duration& store_) : ref_time(clock::now()), store(store_) {}

    ~time_guard()
    {
        store = std::chrono::duration_cast<Duration>(clock::now() - ref_time);
    }

    clock::time_point ref_time;
    Duration&         store;
};

std::vector<bool> sieve0(size_t n)
{
    std::vector<bool> primes(n + 1, true);
    primes[0] = primes[1] = false;
    for (size_t i = 2; i <= n; ++i) {
        if (not primes[i]) {
            continue;
        }
        for (size_t j = i + 1; j <= n; ++j) {
            primes[j] = primes[j] and j % i != 0;
        }
    }
    return primes;
}

std::vector<bool> sieve1(size_t n)
{
    std::vector<bool> primes(n + 1, true);
    primes[0] = primes[1] = false;
    for (size_t i = 2; i <= n; ++i) {
        if (not primes[i]) {
            continue;
        }
        for (size_t j = i * 2; j <= n; j += i) {
            primes[j] = false;
        }
    }
    return primes;
}

std::vector<bool> sieve2(size_t n)
{
    std::vector<bool> primes(n + 1, true);
    primes[0] = primes[1] = false;
    for (size_t i = 2; i * i <= n; ++i) {
        if (primes[i]) {
            for (size_t j = i * i; j <= n; j += i) {
                primes[j] = false;
            }
        }
    }
    return primes;
}

void print_primes(const std::vector<bool>& primes)
{
    for (size_t i = 2; i < primes.size(); ++i) {
        if (primes[i]) {
            std::cout << i << " is prime\n";
        }
    }
}

static constexpr size_t N = 500'000;

int main()
{
    std::chrono::duration<double> time0;
    {
        time_guard clock(time0);
        auto       primes = sieve0(N);
    }
    std::cout << "sieve0(" << N << ") : " << time0.count() << "s\n";

    std::chrono::duration<double> time1;
    {
        time_guard clock(time1);
        auto       primes = sieve1(N);
    }
    std::cout << "sieve1(" << N << ") : " << time1.count() << "s\n";

    std::chrono::duration<double> time2;
    {
        time_guard clock(time2);
        auto       primes = sieve2(N);
    }
    std::cout << "sieve2(" << N << ") : " << time2.count() << "s\n";
}
