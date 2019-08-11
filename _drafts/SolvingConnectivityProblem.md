---
Title: Problem solving, a graph story
---

Recently I've come across this article (https://www.freecodecamp.org/news/bet-you-cant-solve-this-google-interview-question-4a6e5a4dc8ee/)[] about an interview problem and a possible
solution. I found the proposed solution a bit convoluted for the problem (and for a
        whiteboard interview) so I've gave it a try. This is the occasion to speak again
about graph algorithms.

## The problem

The problem is presented using an image of a grid drawn on a piece of paper, and the
target is to find the biggest group adjacent cells of the same color.

Since it's an interview question, we can assume a more practical input, but it doesn't
need to be to far from a basic image. For the rest of this post, I'll consider that we get
the original grid as a matrix of whatever type you can imagine to represent the colors as
long as you can compare them for equality. This approach can even work with a bitmap image
as long as the cells of the grid are regular and adjacent blocs of pixels, including
single pixels.

To avoid diving input the dirty space of two dimensional arrays, we assume our input is a
single dimension array in line first representation (the array is the concatenation of all
        lines of the matrix). As long as we can easily compare cells, we don't need any
transformation on the input.

## Where are the graphs here ?

Connectivity always rings the bell of graphs, even if it's not obvious in this case. So
let's find graph here. If we want to map to a connectivity problem, we'll obviously see
grid cells as vertices, and from which it follows that we'll an edge between two vertices
if the corresponding cells are adjacent and of the same color.

**Do we need a special data structure ?**

We can do without, so I'll avoid it. Just a word of explanation here, most of the time in
graph algorithms, what we need is getting the adjacent vertices from a given vertex, which
is pretty straightforward here (look at the adjacent cells in the grid). We may need to
iterate on vertices (do I need to tell you how ?) or edges (iterate on vertices and for
        each iterate on neighbours). So, this input matrix is all we need.

Now, let's pick an algorithm.

## Candidates algorithms

The most obvious algorithm for detecting connected components in a graph is to perform a
traversal from a random vertex, marking every encountered vertices as belonging to the
same component, then pick the next vertex that is not marked by the previous traversal and
traverse again. It's probably the simplest application of graph traversal. Note that you
can use a depth first (DFS) or breadth first (BFS) version, it works the same.

But honestly, this is not my prefered solution, there's a nice one, pretty short to write,
    thus perfect for coding interview: **Union/Find**.

Before explaining it, few words about the traversal solution: it works, it pretty easy to
understand and thus to explain during an interview, but these solutions still havve some
tricky details, while the algorith I propose you is quiet is straightforward.
