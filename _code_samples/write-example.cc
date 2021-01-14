#include <cerrno>
#include <err.h>
#include <iostream>
#include <string>
#include <unistd.h>

int main()
{
    constexpr char msg[] = "write(2) example\n";
    constexpr int  len   = sizeof(msg);

    int written = 0;
    do {
        // write to stdout the content of msg shifted by the amount already written
        auto w = write(STDOUT_FILENO, msg + written, len - written);
        if (w < 0) {
            // Interrupt ?
            if (errno == EINTR) {
                continue;
            }
            // else die
            err(1, "error while writing");
        }
        written += w;
    } while (written < len);
}
