/*
 * read_stdin.cc
 */

#include <cerrno>
#include <err.h>
#include <iostream>
#include <string_view>
#include <unistd.h>

int main()
{
    int          r;
    const size_t amount = 1;
    char         buff[amount];

    while ((r = read(STDIN_FILENO, buff, amount)) != 0) {
        if (r < 0) {
            // We may have been interrupt, the standard tells us to "retry"
            if (errno == EINTR) {
                continue;
            }
            err(1, "error while reading");
        }
        std::cout << std::string_view(buff, r);
    }
}
