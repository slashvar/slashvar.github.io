---
title: Properly using GNU-Make
---

This small tutorial presents simple smart usage of the GNU version of make. The goal is to guide you in the usage of implicit rules in order to compile C (or C++) small projects. We consider that if you have complex issues, you should use automated tools like auto-tools.

Our rational is that too much Makefile are way too complex for their purpose and rather than explaining howto write recipes rules, we should explain how to avoid writing them !

For all the following examples, we consider that you're using an Unix terminal with a standard shell, and your system provides make and a C compiler (gcc for instance).

## First-step: single file compiling ##

So, consider we have a single C file <tt>hello.c</tt>, like this one:

```C
# include <stdio.h>
# include <stdlib.h>

int main()
{
  printf("Hello World !\n");
  return 0;
}
```

make knows how to build a program made of a single file out-of-the-box. You just have to ask for the program name and it will look for the corresponding C file:

<pre>
shell> ls
hello.c
shell> make hello
cc     hello.c   -o hello
shell> ls
hello  hello.c
shell> ./hello 
Hello World !
shell>
</pre>

This is compiling our code with the minimal set of options (none) and the standard compiler (cc). If some global set-up has been defined on your system, you may see a different invocation.

So, we haven't written any line in a Makefile, and it's already working !

No consider that we want to control a little bit the compiler. We want to set some options like warnings, C standard version and eventually optimization flags. All we need is to set-up a variable named CFLAGS with the various options:

<pre>
shell> rm hello
shell> env CFLAGS=" -Wall -Wextra -std=c99 -O2" make hello
cc  -Wall -Wextra -std=c99 -O2    hello.c   -o hello
shell>
</pre>

First, why do I need to remove the binary program first ? One of the main features of make is to avoid rebuilding already compiled files, so <tt>hello.c</tt> hasn't change since the last time we compile it and make just doesn't see why it should rebuild it. So you need to remove your target (our update your code) in order to rebuild it.

OK, so far, we got a call that is as annoying to type as invoking the compiler directly. We need to put our variable somewhere. Setting it as a global environment is not a good idea, most of theses options may change for each projects. That's where Makefile appears, the simplest Makefile look this:

```make
# Very basic Makefile

CFLAGS= -Wall -Wextra -std=c99 -O2

# marking end of file can avoid problem with
# make parsing strategy if your editor doesn't
# close the line for you.
# END
```

Now, if we invoke make again (after removing <tt>hello</tt>), we got the expected result:

<pre>
shell> ls
Makefile  hello.c
shell> make hello
cc -Wall -Wextra -std=c99 -O2    hello.c   -o hello
shell> 
</pre>

At this point the last things you may want to control is which compiler to run (cc should be OK for most cases, but if your not sure of what cc is referring to, you may want to set the compiler using the CC variables:

```make
# Very basic Makefile

CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2

# END
```

And now we get:

<pre>
shell> ls
Makefile  hello.c
shell> make hello
gcc -Wall -Wextra -std=c99 -O2    hello.c   -o hello
shell> 
</pre>

## Compilations steps ##

If you have basic C knowledge, you, you probably know the compilation steps. So in short you have:

* pre-processing (<tt>cpp</tt>)
* compilation (producing object-code, <tt>cc -c</tt>)
* linking

<tt>make</tt> understand these steps and got some variables for each of them. When invoked for a single file program, all steps are ran as one single step, otherwise, pre-processing (unless your in some strange cases) is invoked together with the compilation step.

You can control a various set of options passed at each steps, and we'll see that correctly separating theses options is important in order to get the compilation done.

So, here is a template Makefile with a placeholder for each building variable:

```make
# template Makefile

# Setting the compiler and the default linker program
CC=gcc
# options for pre-processor (-I, -include, -D ... )
CPPFLAGS=
# main compilation options
CFLAGS= -Wall -Wextra -std=c99
# Linker options (probably always empty)
LDFLAGS=
# libs and path for linker
LDLIBS=

# END
```

In order to correctly use make, you need to understand how to compile code, not how make does. So, if you're not familiar with the C compilation schema, we strongly encourage you to open a good C programming book.

## Multiple Files ##

Now, we extend our example so we need to compile several files in order to build our program. For our example, we build a small program with the following files:

<pre>
main.c  median.c  median.h  sort.c  sort.h
</pre>

### Simple build ###

In order to produce our program (<tt>main</tt>) we need to produce a <tt>.o</tt> file for each <tt>.c</tt> file.

Once again, we'll use predefined rules in order to build our program. Basically we just need to describe the dependency between the target program and the object files (<tt>.o</tt>). Let's try this simple <tt>Makefile</tt>:

```make
main: main.o median.o sort.o

# END
```

Just ask make to produce <tt>main</tt>:

<pre>
shell> make main
cc    -c -o main.o main.c
cc    -c -o median.o median.c
cc    -c -o sort.o sort.c
cc   main.o median.o sort.o   -o main
shell>
</pre>

Again, we would like to use some compilation options, so we add our variables:

```make
CPPFLAGS=
CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS=

main: main.o median.o sort.o

# END
```

After cleaning the current directory, we try to build our code again:

<pre>
shell> make main
gcc -Wall -Wextra -std=c99 -O2   -c -o main.o main.c
gcc -Wall -Wextra -std=c99 -O2   -c -o median.o median.c
gcc -Wall -Wextra -std=c99 -O2   -c -o sort.o sort.c
gcc   main.o median.o sort.o   -o main
shell>
</pre>

### Cleaning ###

Tired of manually cleaning your working directory, this is the occasion to build a cleaning recipe:

```make
clean:
	rm -f *.o      # remove object files
	rm -f main     # remove main program
```

<pre>
shell> ls
Makefile  main.c  median.c  median.o  sort.h
main	  main.o  median.h  sort.c    sort.o
shell> make clean
rm -f \*.o      # remove object files
rm main        # remove main program
shell> ls
Makefile  main.c  median.c  median.h  sort.c  sort.h
shell>
</pre>

### Getting more organized ###

OK, now we want to organize our <tt>Makefile</tt> a little bit more. First, we need list of source files and object files. First the source:

```make
SRC= main.c median.c sort.c
```

Now, we can produce the list of object files:

```make
OBJ= ${SRC:.c=.o}
```

The variable <tt>OBJ</tt> is defined using the content of the variable <tt>SRC</tt> where the string <tt>.c</tt> is replaced by the string <tt>.o</tt>. Now that we have this list, we can rewrite our <tt>Makefile</tt>:

```make
# Makefile

CPPFLAGS=
CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS=

SRC= main.c median.c sort.c
OBJ= ${SRC:.c=.o}

main: ${OBJ}

clean:
	rm -f ${OBJ}   # remove object files
	rm -f main     # remove main program

# END
```

### Dependencies and project partial build ###

One of the most important feature of make (for developers) is that it is able to choose which file needs to be rebuild (using modification dates on files.)

Let's try this a little bit:

<pre>
shell> make
gcc -Wall -Wextra -std=c99 -O2   -c -o main.o main.c
gcc -Wall -Wextra -std=c99 -O2   -c -o median.o median.c
gcc -Wall -Wextra -std=c99 -O2   -c -o sort.o sort.c
gcc   main.o median.o sort.o   -o main
shell> make
make: 'main' is up to date.
shell> touch median.c
shell> make
gcc -Wall -Wextra -std=c99 -O2   -c -o median.o median.c
gcc   main.o median.o sort.o   -o main
shell>
</pre>

That's nice, but what if I modify my headers ?

<pre>
shell> touch median.h
shell> make
make: 'main' is up to date.
shell>
</pre>

make doesn't know the dependency link between headers and code, we need to tell him ! On a big project, it can be painful to list all dependencies, but *someone* is already able to tell make these dependencies: your compiler !

GCC (and clang) are able to generate dependency while compiling your code. Since we only need theses dependencies when rebuilding, we can consider that they will be available *later*. For that we use <tt>-MMD</tt> option of GCC: it creates a <tt>.d</tt> file for its input that we can include in our <tt>Makefile</tt>. I let you read your favorite compiler documentation for more information. This flag is related to the pre-processor, so we add it in the <tt>CPPFLAGS</tt>  variable.

```make
# Makefile

CPPFLAGS= -MMD
CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS=

SRC= main.c median.c sort.c
OBJ= ${SRC:.c=.o}
# add list of dependency files
DEP= ${SRC:.c=.d}


main: ${OBJ}

# include dependency files
-include ${DEP}

clean:
	rm -f ${OBJ}   # remove object files
	rm -f ${DEP}   # remove dependency files
	rm -f main     # remove main program

# END
```

New stuff:

* New flags in <tt>CPPFLAGS</tt>
* <tt>DEP</tt> variable listing dependency files produced by the compiler
* <tt>-include</tt> directive telling make to include dependency files
* <tt>clean</tt> rule extended to remove these new files

Note that we add the <tt>-include</tt> directive **after** the rule for main. The default target of make is the first rule of the file, so if you want to be able to invoke make without specifying a target, it must be the first one.

<pre>
shell> ls
Makefile  main.c  median.c  median.h  sort.c  sort.h
shell> make
gcc -Wall -Wextra -std=c99 -O2 -MMD  -c -o main.o main.c
gcc -Wall -Wextra -std=c99 -O2 -MMD  -c -o median.o median.c
gcc -Wall -Wextra -std=c99 -O2 -MMD  -c -o sort.o sort.c
gcc   main.o median.o sort.o   -o main
shell> touch median.h
shell> make
gcc -Wall -Wextra -std=c99 -O2 -MMD  -c -o main.o main.c
gcc -Wall -Wextra -std=c99 -O2 -MMD  -c -o median.o median.c
gcc   main.o median.o sort.o   -o main
shell> ls
Makefile  main.c  main.o    median.d  median.o	sort.d	sort.o
main	  main.d  median.c  median.h  sort.c	sort.h
shell>
</pre>

### Respecting convention ###

Usually, a <tt>Makefile</tt> should provide some standard targets, here is two of the most common:

* <tt>all</tt>: the default job, should also be defined as the first one
* <tt>clean</tt>: should remove all files produced by the building process

We already have a clean rule, we just want to mark it as *phony*: the recipe for the rule **must** be executed completely each time you invoke it.

Now, our <tt>Makefile</tt> looks like this:

```make
# Makefile

CPPFLAGS= -MMD
CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS=

SRC= main.c median.c sort.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	rm -f ${OBJ}   # remove object files
	rm -f ${DEP}   # remove dependency files
	rm -f main     # remove main program

-include ${DEP}

# END
```

### Using libraries ###

Some compilers require that you respect a specific order when providing static
object (<tt>.o</tt> files) and dynamic objects (specify with <tt>-l</tt>.)

While it's interesting to understand how to correctly build the linker command,
when compiling your project, you just want it to work. Again, <tt>make</tt>
knows how to deal with that correctly, the only things you need is to put the
right information in the right variables.

So consider your code now requires to be linked with the math lib
(<tt>-lm</tt>), all you have to do is add <tt>-lm</tt> in <tt>LDLIBS</tt>
variable.

```make
# Makefile

CPPFLAGS= -MMD
CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS=
LDLIBS= -lm

SRC= main.c median.c sort.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	rm -f ${OBJ}   # remove object files
	rm -f ${DEP}   # remove dependency files
	rm -f main     # remove main program

-include ${DEP}

# END
```

When using libs that are not part of the standard distribution of C (so anything
that is not <tt>-lm</tt>) you should first check if the lib provides flags
through <tt>pkg-config</tt>.

Let's consider you want to use SDL, first look if it is listed by <tt>pkg-config</tt>:

<pre>
shell> pkg-config --list-all | grep sdl
sdl                                     sdl - Simple DirectMedia Layer is ...
sdl2                                    sdl2 - Simple DirectMedia Layer is ...
</pre>

Here we can see that we have some configurations for SDL and SDL2, choose your
own (say SDL). Let see what we can get:

<pre>
shell> pkg-config --cflags sdl
-D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/SDL
shell> pkg-config --libs  sdl
-lSDL -lpthread
</pre>

The first call provides stuff for <tt>CFLAGS</tt> (in fact all theses flags
should be in <tt>CPPFLAGS</tt> but <tt>pkg-config</tt> doesn't make the
difference.) The second call provides flags for the linker.

So, we can use this in our <tt>Makefile</tt>, of course don't copy/paste
the output of the command, but directly call it in the variable definition
but <tt>pkg-config</tt> doesn't make the difference.) The second call
provides flags for the linker.

So, we can use this in our <tt>Makefile</tt>, of course don't copy/paste the
output of the command, but directly call it in the variable definition
(<tt>make</tt> supports shell syntax in variable definitions.)

```make
# Makefile

CC=gcc
CPPFLAGS= -MMD
CFLAGS= -Wall -Wextra -std=c99 -O2 $(pkg-config --cflags sdl)
LDFLAGS=
LDLIBS= -lm $(pkg-config --libs sdl)
```

## Conclusion ##

As you can see, we have set up a simple <tt>Makefile</tt> that is able to
compile a project with a simple architecture (all source in the same directory)
without writing recipes (for compiling.)

When writing a <tt>Makefile</tt> for a C project (or C++) you almost never need
to write your own recipes. A good <tt>Makefile</tt> is short and contains as few
rules as possible. If you can't rely on implicit rules, you may probably want to
use a more advanced tools like the autotools or cmake.

If you want to know more about available implicit rules, the best place is the
GNU Make manual:

* (GNU Make Manual)[https://www.gnu.org/software/make/manual/make.html]
* (GNU Make Manual | Using Implicit Rules)[https://www.gnu.org/software/make/manual/make.html#Implicit-Rules]

If you're using one of the BSD variant of <tt>make</tt>, you'll probably find
usefull information on the proposed documentations for respective project,
FreeBSD provides a (tutorial for PMake)[https://www.freebsd.org/doc/en_US.ISO8859-1/books/pmake/] (Portable Make, the BSD make) and the manual
page for make in OpenBSD is also a good source of information.


