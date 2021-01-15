/*
 * basic_cat.cc : a quick example on calling syscall
 */

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string_view>
#include <unistd.h>

/*
 * my_cat(fd) reads chars from fd and write them to the standard output
 *
 * Exit the program with appropriate error message if read(2) fails
 */
void my_cat(int fd)
{
    // Buffer for read with
    constexpr size_t buff_size = 1024;
    char buff[buff_size];

    // read return value
    int ret_val;
    while ((ret_val = read(fd, buff, buff_size)) != 0) {
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
    std::cout.flush(); // flush output
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
