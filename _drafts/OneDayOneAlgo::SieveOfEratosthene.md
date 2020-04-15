---
Title: "One Day, One Algorithm: Sieve of Eratosthenes"
---

I've made a week of articles about technical subjects. Now, after another quarantine week doing nothing (I'm off),
I wanted to write something else. In order maintain a bit my brain, I will try write about classic algorithms (one
per day, as the title says).

So let's start with a pretty old one, the sieve of Eratosthenes, an algorithm that will let you enumerate prime
numbers until a fixed bound. I said old, Eratosthenes lived between 276BC and 194BC, yes more than two thousands
years ago, but there is no better one to list prime numbers.

## The algorithm

The idea is pretty simple, once you have a prime number, you can eliminate all its multiple. So we start with an
array of booleans, the cell `i` indicates if `i` is prime, but at the beginning all are consider prime. We just
mark `0` and `1` as false.

The algorithm is based on a loop over the array, if the current cell `i` contains true, `i` is prime and we have to
mark as false all it's multiples with a second (inner) loop. This is quiet simple, isn't it ?

Why it works ? Pretty easy, when reaching cell `i`, we have eliminate all the multiple of the prime numbers smaller
than `i`, so either `i` is prime, either it is marked as non-prime.

## First implementation

Let's implement that in C++, we will start with the most straighforward version. The boolean vector will be our
result as it is enough to list the prime numbers. Let's have a look at the code:

```c++
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
```

The vector is created with enough room to store the information on prime numbers up to `n` (included) and as we
said, at the beginning, all number are considered prime (but `0` and `1`). Then, we iterate from `2` to `n`. If the
current value is not prime, we continue to the next one. Otherwise, we scan the array and mark as false all
multiple of `i` (keeping false those that are already marked).

We can check our result by printing the prime numbers (until a reasonnable bound) and check them by hand (or
looking at a table online). Here is a possible printing function:


```c++
void print_primes(const std::vector<bool>& primes)
{
    for (size_t i = 2; i < primes.size(); ++i) {
        if (primes[i]) {
            std::cout << i << " is prime\n";
        }
    }
}
```

## Optimization and performances

### Measure

This implementation works but we can probably improve a bit the algorithm. But before that, we need to measure
performances improvements. Let's build a simple tool to measure performances, using RAII and `std::chrono`:

```c++
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
```

Now we can write a main function to test our code:

```c++
static constexpr size_t N = 500'000;

int main()
{
    std::chrono::duration<double> time0;
    {
        time_guard clock(time0);
        auto       primes = sieve0(N);
    }
    std::cout << "sieve0(" << N << ") : " << time0.count() << "s\n";
}
```

Now you just have to add includes, compile and run.

### First optimizaton

So, what can be improved ? When trying to improve that kind of algorithm, we can't expect a deep change, we need to
traverse all the numbers. What we're looking for is unecessary steps and computations. And we have one obvious
source of useless computations in the inner loop. We don't need to search all intergers in order to find multiples
of the current number, we can directly jump from multiple to multiple by simple additions. So, here is our new
version:

```c++
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
```

We can duplicate the measurement part in the main function and run it to compare both version:

```
> clang++ -Wall -Wextra -std=c++17 -O3 -o prime prime.cc
> ./prime
sieve0(500000) : 29.3604s
sieve1(500000) : 0.00301103s
```

Impressive, isn't it ? Can we find the reason of such a difference ? First, we removed modulus operations that are
way more expensive than additions (additions that were there before anyway). The real improvement is on the number
of steps of the inner loop, which could have a large impact since it is repeated quiet a lot.

Can we do better ?

### Second optimizaton

There is another possible source of useless steps. We start the inner loops at `i * 2` (the first multiple of `i`)
but, based on the principle of the algorithm, we know that we have marked all multiple of the previous prime
numbers, among them the multiple of `i`. In fact, we know that we have marked all multiple of `i` smaller than
`i * i`. Let put that in practice:

```c++
std::vector<bool> sieve2(size_t n)
{
    std::vector<bool> primes(n + 1, true);
    primes[0] = primes[1] = false;
    for (size_t i = 2; i <= n; ++i) {
        if (not primes[i]) {
            continue;
        }
        if (i * i > n) {
            break;
        }
        for (size_t j = i * i; j <= n; j += i) {
            primes[j] = false;
        }
    }
    return primes;
}
```

Let's run the three and compare them:

```
> ./prime
sieve0(500000) : 29.3183s
sieve1(500000) : 0.00291156s
sieve2(500000) : 0.0015131s
```

Another nice improvement, not as impressive but still pretty good. Not that I have added another little detail that
stop the main loop earlier, we could incorporate it in the loop condition, that will be cleaner, but I wanted it to
be more obvious.

A final version, cleaner:

```c++
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
```

## Conclusion

This is a classic, in fact, if you look at the wikipedia page, you will see that the provided version look pretty
close to my final one. They give the complexity: `O(n log log n)` of this version, while the original one is
quadratic.

Can we improve it ? There exists various versions, one tries to solve the memory issue, as the space complexity is
`O(n)`.

The implementation of `std::vector<bool>` is using an array of bits. It may be possible that using bitwise
operation we can improve the inner loop a bit further, but it won't break the complexity as did the first
optimization. We can also try to play a bit with the array to only consider odd numbers, reducing the size of the
array.
