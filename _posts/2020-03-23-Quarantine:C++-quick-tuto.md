---
title: Quarantine Quick C++ Tutorial
---

Being stuck at home, we all have some extra time to work on various subjects we never found time for. I've decided to write small C++ tutorials, I can't guarantee to write a new one everyday, but I'll try. The purpose is to quickly explore a subject in less than 30min.

For the first one, I'll pick my favorite beginner topic: C++ is not (that) hard. This quick tuto will try to demonstrate that you can write C++ without spending too much time on complex topic such as memory management.

## Tiny sort

For this tuto, we will implement a simplified version of the unix command `sort`. This command read lines from the standard input (or a file, but let's start with the standard input), sort them and write them sorted on the standard output. The command has more options but we don't want to write an option parser.

We can implement a quick version in python, just to see where we are going.

```python
from sys import stdin
if __name__ == '__main__':
    lines = []
    for line in stdin:
        lines.append(line.strip('\n'))
    lines.sort()
    for line in lines:
        print(line)
```

The code is really simple, of course, the C++ version will be longer, but not that much.

## Setting up the scene

First, we need to prepare our program and include the needed components of the standard library.

```c++
// Algorithms (for sort)
#include <algorithm>
// Stream input and output
#include <iostream>
// Vector containers
#include <vector>

// The program entry point
int main()
{

}
```

If you want to know more about these includes, I strongly encourage you to use http://cppreference.com and explore this fine documentation.

We don't have code yet, but it could be a good idea to start compiling your program. Being an old fashioned dev, I prefer compiling from a terminal. Since we are working with a single file, we don't need a build manager. I'm using `clang++` as my compiler but `g++` will work exactly the same here. Let's assume the code is in a file called `my_sort.cc` and we want to produce a binary called `my_sort`:

```
> clang++ -Wall -Wextra -std=c++17 -O3 -o my_sort my_sort.cc
```

This command should produce a program called `my_sort`. We build with the most common warning flags (`-Wall` and `-Wextra`), we use optimizations at level 3 (not for the perf, but for the extra warnings it brings) and we assume C++17 standard (but C++11 or C++14 should work too).

## Reading the standard input

C++ has several tools to do I/Os, but the easiest to use are the streams library. Since we are just reading lines, the best tool at hand is `std::getline`. As a first demonstration, we will read and reouput the line from the standard input:

```c++
int main()
{
    for (std::string line; std::getline(std::cin, line);) {
        std::cout << line << std::endl;
    }
}
```

`std::cin` is the standard input and `std::cout` the standard ouput. The loop is pretty similar to what we've done in python, while `std::getline` don't return false (it's not exactly what it returns, but that will be enough for today) we read a line and assign it to `line`.

Note the usage of the operator `<<` to output what we have read.

We can try that piece of code:

```
shell> ls
my_sort.cc my_sort.py
shell> echo "line 1" > foo
shell> echo "line 2" >> foo
shell> clang++ -Wall -Wextra -std=c++17 -O3 -o my_sort my_sort.cc
shell> cat foo | ./my_sort
line 1
line 2
```

## Storing the lines

Since we need to sort the lines, we need to sort store them. We need the simpliest form of container, a vector.

```c++
int main()
{
    // A vector of strings
    std::vector<std::string> lines;

    // Read and store the lines
    for (std::string line; std::getline(std::cin, line);) {
        // the line is copied in the vector
        lines.push_back(line);
    }
    // Print the lines
    for (const auto& line : lines) {
        std::cout << line << std::endl;
    }
}
```

The storing part is pretty straightforward, we push at the back the line we have just read.

For the output, we use a form of _for each_ loop. The variable is declared as `const auto&` meaning that we don't need to modify it (`const`) and we don't want a copy but just a reference (the `&`) and since we are lazy we don't want to write `std::string` again and use `auto` instead.

This code should have the same output as the previous example.

## Sort ?

The STL (Standard Template Library) provides a lot of interesting tools and algorithms. We can find a sort function (`std::sort`, what a surprise).

But you can't write `std::sort(lines)`, it doesn't work that way. Algorithms takes iterators, a begin and an end. In our case, we want to sort the whole vector, for that we will use the two functions that provide the begin and end of the vector. So all we have to do is to add the following line:

```
    std::sort(lines.begin(), lines.end());
```

`std::sort` can be used to sort only a part of a container or can take a comparison function, read the doc if you want to know more about it.

## Putting everything together

Now we can see or full `my_sort` command:

```c++
// Algorithms (for sort)
#include <algorithm>
// Stream input and output
#include <iostream>
// Vector containers
#include <vector>

// The program entry point
int main()
{
    // A vector of strings
    std::vector<std::string> lines;

    // Read and store the lines
    for (std::string line; std::getline(std::cin, line);) {
        // the line is copied in the vector
        lines.push_back(line);
    }
    // Sort the lines
    std::sort(lines.begin(), lines.end());
    // Print the lines
    for (const auto& line : lines) {
        std::cout << line << std::endl;
    }
}
```

We can compile and try our code:

```
shell> seq 10 | sort -R > random-lines
shell> cat random-lines
6
9
8
3
5
1
4
10
7
2
shell> clang++ -Wall -Wextra -std=c++17 -O3 -o my_sort my_sort.cc
shell> cat random-lines | ./my_sort
1
10
2
3
4
5
6
7
8
9
```

Note that we are sorting string, hence `10` is before `2`, just try with `cat random-lines | sort` and you will get the same output.

## "I've learned that in C++ you must manage memory, are we safe here ?"

Short version: Yes !

In C++, objects that go out of scope are implicitly destroyed. For objects containing resources (like a vector or a string), if they are properly implemented (hopefully this is the case for those provided by the STL) they will release their resources when destroyed.

Can we be sure ? Yes, we can try to run our code with tools to check for memory leak. The simplest way is to use the address sanitizer provided by `clang` and `gcc`. If we have memory leak at the end, we will see some errors:

```
shell> clang++ -Wall -Wextra -std=c++17 -fsanitize=address -O3 -o my_sort my_sort.cc
shell> cat random-lines | ./my_sort
1
10
2
3
4
5
6
7
8
9
```

## Going further ?

You can push this code a little bit further and take a file name as argument, and read from it rather than from the standard input. Or you can try to see how to build a numeric sort.

## Goodbye

That's all for today, that was an easy one, I will try to write something a bit more advanced tomorrow, maybe something with template or RAII.
