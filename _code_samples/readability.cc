unsigned some_computation(unsigned x)
{
    unsigned r = x;
    while (x / r < r)
        r = (r + x / r) / 2;
}

unsigned int_sqrt(unsigned x)
{
    unsigned r = x;
    while (x / r < r) {
        r = (r + x / r) / 2;
    }
    return r;
}

unsigned some_computation2(unsigned x)
{
    // some code ...
    unsigned r = int_sqrt(x);
    // more code ...
}