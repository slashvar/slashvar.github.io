/*
 * basic_cat.cc : a quick example on calling syscall
 */

// could be convenient
#include <string_view>

// we will use std::cout to write to the standard output
#include <iostream>

// the libC header for open
#include <fcntl.h>

// the libC header for read
#include <unistd.h>

// for error handling
#include <err.h>
#include <errno.h>

/*
 * my_cat(fd) reads chars from fd and write them to the standard output
 *
 * Exit the program with appropriate error message if read(2) fails
 */
void my_cat(int fd)
{
    // Buffer size
    const size_t buf_size = 1024;
    // Buffer for read with
    char buff[buf_size];

    // read return value
    int ret_val;
    while ((ret_val = read(fd, buff, buf_size)) != 0) {
        if (ret_val < 0) {
            // EINTR is not an error, we should just retry
            if (errno == EINTR) {
                continue;
            }
            // Otherwise we die with err
            err(1, "error while reading");
        }
        std::cout << std::string_view(buff, ret_val);
    }
    std::cout << std::endl; // flush and skip a line
}

/*
 * Take the file name as first parameter, opens it and calls my_cat
 */
int main(int argc, char* argv[])
{
    if (argc < 2) {
        // requires a parameter
        errx(1, "usage: %s <file>", argv[0]);
    }
    if (int fd = open(argv[1], O_RDONLY); fd >= 0) {
        my_cat(fd);
        // close the file
        close(fd);
    } else {
        // Something went wrong
        err(1, "can't open %s", argv[1]);
    }
}
