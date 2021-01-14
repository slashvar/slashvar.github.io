#include <cerrno>
#include <err.h>
#include <iostream>
#include <string>
#include <unistd.h>

/*
 * demo(fd) reads bytes from fd and output a string
 */
std::string demo(int fd)
{
    // We need a buffer
    constexpr size_t buff_size = 16;
    char             buff[buff_size];

    // the output string
    std::string output {};

    // read return value
    int r = 0;

    // end-of-file is reached when read returns 0
    while ((r = read(fd, buff, buff_size)) != 0) {
        if (r < 0) {
            // read failed
            if (errno == EINTR) {
                // Reading was interrupt, just restart, see read(2)
                continue;
            }
            // for other error, let's just display it and leave the program
            err(1, "error while reading");
        }
        // insert what was read at the end of output
        output.insert(end(output), buff, buff + r);
    }
    return output;
}

int main()
{
    std::cout << "Reading from stdin ...\n";
    auto output = demo(STDIN_FILENO);
    std::cout << "Content:\n";
    std::cout << output << std::endl;
}
