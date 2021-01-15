---
Title: Introduction to system programming
---

System programming is one of the major topics of software engineering. The boundary of what is _system_ in
programming is often fuzzy, but the concept behing may serve engineers in many context. It's part of my day to day
job and, in my previous carreer, it was one of my teaching topics.

There are various way to define system programming, but there are some common aspects that can be used to draw a
first scope of the topic:

* Interractions with files system
* Communications between processes
* Processes management
* Interraction with the operating system or underlying hardware components

I tend to describe system programming as **buidling programs that interract with non-human actors**.

So, I wanted to write some posts (let say, I will start with one) about system programming in general, putting down
theory and experience.

## How?

A large part of system programming is about system interractions, you need to choose a system. I will use POSIX
like systems and Linux (but I will try to stick to POSIX as much as possible).

The second question is about the programming language. Lot's of material around system programming are using C, for
a lot of good reasons, but that's not mandatory. Theoretically, you can do system programming with a lot of
different languages. I believe that C++ could be a good choice here, as it is close enough to C and provides some
comfortable abstractions.

So, if you want to run the provided code or make your own experiments, you will need POSIX like system (Linux,
MacOS, FreeBSD, OpenBSD ... ) with a compiler supporting C++17 (I am using `clang` 10), a terminal and a text editor.
A version of `make` can be useful, but you can do without.

## System calls

The lower level of interractions with the system for a userland program are syscalls. For the programmer, a syscall
looks like a regular function, but behind the scene it triggers a context switch with the system. Since it's an
introduction, we won't dive deeper and use them as _normal functions_. Syscalls are described in section 2 of the
man pages (some function of section 3 are in fact wrappers around syscalls too). You can try `man 2 read` in your
shell for an example. I will use the usual convention `read(2)` that indicates that the man page of the function
`read` is in section `2`.

Most syscalls returns an `int` with negative values indicating failures. They all set the global symbol `errno` to
the corresponding error code. The meaning of the error code for the current function is described in the man page
of the function, an example from the man page of `read`:

```
ERRORS
     The pread(), read(), and readv() calls will succeed unless:

     [EAGAIN]           The file was marked for non-blocking I/O, and no data were ready to be read.

     [EBADF]            fildes is not a valid file or socket descriptor open for reading.

     [EFAULT]           Buf points outside the allocated address space.

...
```

### A first example with `read(2)`

`read(2)` is a good syscall examples. So, I will start with a small demo function that will read bytes from a file
descriptor (more that later) and add the content to a `std::string`. I will use this example to illustrate both the
way to use `read(2)` and the way to handle errors.

```c++
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
```

This function uses `read(2)` to read bytes from `fd` into `buff` and it reads at by blocks of atmost `buff_size`
bytes, it stores the read bytes into `output` and continue until `read(2)` returns `0`, indicating the end-of-file.

First, `read(2)` is a generic reading syscall, it can read from any readable resources (like files or terminal). It
will read as much as it can in one shot (but no more than the amount asked for) and will block if no data are
available. If you are reading from a file, you will always get the amount you were asking for until you reach the
end. But for stream like resources (terminal, pipes, sockets ... ) `read(2)` will block until data are available
and returns as soon as it can read something, even if there are less bytes than what you asked for. It will then
return the amount of bytes read. If it return `0`, it means that you reach the end of the resource.

If something goes wrong, `read(2)` will return a negative value (usually `-1`) and set `errno` to the appropriate
error code. In some cases, you may be interrupt while waiting for inputs, and in that case the error code will be
`EINTR` and you should resume reading. In case of non-blocking resources, you may also get `EAGAIN` indicating that
no data where available.

In our demo, if the error is not `EINTR`, we _die_ using `err(3)` that will print the name of the program, your
message (using `printf` like format string) and the message corresponding to `errno` value.

You can try this function with the standard input (`STDIN_FILENO`), like in the following example:

```c++
int main()
{
    std::cout << "Reading from stdin ...\n";
    auto output = demo(STDIN_FILENO);
    std::cout << "Content:\n";
    std::cout << output << std::endl;
}
```

You will need to include the missing header to get a working program:

```c++
#include <cerrno>
#include <err.h>
#include <iostream>
#include <string>
#include <unistd.h>
```

In a terminal, the _end of file_ is reached when the user press `^D`.

### An example using `write(2)`

`write(2)` is very similar to read in its usage, I let you read the code sample:

```c++
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
```

## Files

Files manipulations are very common in system programming. We may distinguish operations on the content of the files
(like reading or writing) and operations on the files themselves and the filesystem in general (listing content of
directories, copying files, getting informations ... )

Reading and writing content belongs to input/output operations (I/Os) and are often generic with regards to their
targets (it could be a file, but also a terminal, a pipe, a network socket). In POSIX systems, the underlying
concept used to manipulate the content of files are called **file descriptors**. Languages and their standard
library may provide some abstractions above it, but that's what you have behind.

### File descriptors

A file descriptor is an integer that will serve as a handle for an I/O resource. You can think of it as an index to
an array of resources managed by your system. Any time you want to perform an operation on that resource, you will
call a syscall with at least the file descriptor as a parameter.

For example, to close an already opened resource `fd`, you will call `close(fd)`, to read some bytes you will call
`read(fd, buff, amount)` and so on. All I/O resources can be used with at least `close(2)`, 'read(2)` or
`write(2)`. Some resources may support other operations depending of their kind.

At startup, your program will have, at least, three file descriptor already opened:
* `STDIN_FILENO` : the standard input
* `STDOUT_FILENO`: the standard output
* `STDERR_FILENO`: the standard error output

Those descriptors are usually mapped to the terminal but can be redirected to some other resources by the parent
process or the process itself.

Since file descriptors are not limited to files, you can obtain one from several syscalls, but the simplest way to
get a file descriptor is to open a file, using `open(2)`.

The following example, start with the first demo function but rather that accumulating read content in a string we
will print it out directly, I won't use `write(2)` for that to keep the focus of the example, we will just use the
C++ streams and some `std::string_view`. The only _new_ part of the following example is the use of `open(2)` to
open our file (let's discuss it after the code).

```c++
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
```
So, take a look at the `open(2)` call. We gave it a string (more precisely a null-terminated array of `char`) and
what looks like a flag. It will try to open the corresponding file, returning a file descriptor. As for other,
negative values indicate errors. `open(2)` exists with a three parameters version, but the third parameter is only
usefull if you may create opened file.

The second parameter uses a classical _trick_ to combine flags. Each possible flags as a unique single bit set and
thus by combining the value using a bit by bit or (`|`) you well get an integer combining the various flag, you can
then check if a flag is set by masking this integer with the flag using an and (`&`). In our case, we want to open
the file in read-only mode, so we only need the flag `O_RDONLY`.

If you want to open a file for writing, creating it if it doesn't exist and truncate it otherwise, you will call
`open(2)` like this:

```c++
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
```

The last parameter will be used for the file permissions, masked by the `umask` of owner of the process. This mode
is usually expressed in octal (hence the leading `0`) where each digit will control the permissions for the owner,
the group and other users. Usually, the `umask` value is properly set and allowing read and write for everyone
(`0666` mode) is OK as it will be reduced by the mask to read/write for the owner and read only for other users
(including the group). Unless you need more specific permissions, you follow this convention. Read the manual pages
of `umask(2)` and `chmod(2)` for more information.

Some additional flags are available (see `open(2)`), one important flag is `O_CLOEXEC`. We will discuss the topic
later (when dealing with process management), what you need to know is that if your process may execute (replace
itself with) another program, by default, opened files will remain open for the executed program. The flag
`O_CLOEXEC` indicates that you the opened file to be closed when calling one of the `exec` functions.

### File content manipulation abstractions

File descriptors, `read(2)`, `write(2)` and other operations are the underlying mechanisms to manipulate files.
While you should be able to do anything you want with them, they are not very convenient in most cases.

Language libraries usually provide some portable abstractions. In C++ we have the operations from the C library
(from `stdio`) and the streams. Exploring them is out-of-scope, but here are the main benefits:

* **Portability:** higher level operations, while they rely on underlying system operations, are usually
cross-plateform
* **Buffering:** most higher level operations rely on buffered I/Os to minimize the amount of context switches with
the system.
* **Formatting:** this is usually the most visible extensions

### Paths, directories and file manipulations

For this part, I will choose to use the abstraction from C++17 (`std::filesystem`). While, for content, the core of
the operations, abstractions or not, are about reading and writing bytes, for file manipulations, the underlying
API is a bit more complex. And on top of that, manipulating path means manipulating strings. C++ offers nice
abstractions (using `std::string` or ``std::filesystem::path`) to avoid the cumbersom part of string manipulations.

The core and most important addition brings by `std::filesystem` is `std::filesystem::path`. An abstraction over
path that provides easy manipulations and some classical operations (path validation, file status, creations and
destructions ... )

For the rest of this part, I will follow cppreference practice and consider that we use the following namespace
shortcut:

```c++
namespace fs = std::filesystem;
```


