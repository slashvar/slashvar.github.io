/*
 * Simple syscall example: open a file
 */

#include <cerrno>
#include <err.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        // No args, leave with an error message
        errx(1, "usage: %s <file>", argv[0]);
    }
    // Try to open the file
    if (int fd = open(argv[1], O_RDONLY); fd < 0) {
        // negative value means error, let err(3) display an appropriate error message
        err(1, "error while opening %s", argv[1]);
    } else {
        // OK, file is opened, we have nothing else to do
        std::cout << "We have opened " << argv[1] << std::endl;
        // Let's properly close it
        close(fd);
    }
    return 0; // not mandatory
}
