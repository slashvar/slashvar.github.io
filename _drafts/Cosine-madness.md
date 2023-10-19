---
Title: Computing Cosine or simple code can still be surprising
---

In this post I'll go back to something I haven't done for a while, digging into a simple computation function. Of course there is a little story behind.

## Cosine similarity ##

This is a short presentation of cosine similarity for those that are not familiar with it. In short, this is an easy to compute similarity between two vectors of same dimensions. The value ranges from `-1` to `1` (but is often truncated to fit in the `[0...1]` range), `1` means the two vectors are colinear (they point in the same direction, so they are very similar), `0` means they are orthogonal (so they are very different) and `-1` means they point in opposite direction.

This is very useful when dealing with vectors produced by some ML process. For example, you can use some language model to produce vectors representing some texts and then compare them using the cosine similarity. If the similarity tends to `1`, both texts are supposed to be _closed_ to each other and if it tends to `0` (replacing negative values with `0`), them they are different.

The web being full of literature on this topic, you will probably find some more detailled presentations as this is not the point of this post. So, let's move on to my little story ...

## Down the cosine hole ... ##

Few days ago, I was doing some tests on a _vectorizer_ service, fixing some details on a  gRPC proto and other boring stuff. Out of ideas for my tests, I started to push the same random texts several time and compute the resulting cosine similarity (much more readable than the 512 floats of those vectors). Computing the vectors for the same text, should produce the same vectors, and thus the similarity should be `1.0`. Guess what, it was not. Oh, not a big error, but that was surprising (before you asked, yes the vectors were the same).

So, I found the code (in go) for the cosine function used in our project and start thinking about what can be wrong.

So, computing cosine for two vectors `u` and `v` is done by computing the dot product of `u` and `v` and divide it by the product of the norm of the vectors.

Here is the code I found in our project:

```golang
func Cosine(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, errors.New("invalid arguments")
	}
	var dotProduct float64
	for k, x := range u {
		dotProduct += float64(x) * float64(v[k])
	}
	sumU := 0.0
	for _, x := range u {
		sumU += float64(x * x)
	}
	sumV := 0.0
	for _, x := range v {
		sumV += float64(x * x)
	}
	magnitude := math.Sqrt(sumU) * float64(sumV)
	if magnitude == 0 {
		return 0.0, nil
	}
	cos := dotProduct / magnitude
	if cos < 0 {
		return 0, nil
	}
	return cos, nil
}
```

The input vectors are using `float32` but the result is computed as `float64` due to some constraints. This is not important, isn't it?

There are two issues with this code, one is _obvious_ when you're used to floating point problem. But don't go too fast ...

In the past, I lost enough time around sum of vector of floats, so I jumped directly on my favorite _Divide and Conquer_ sum algorithm. In the process, I fixed the two errors without even seeing them ... Here is my first implementation:

```golang
func dotProd(v1, v2 []float32) float64 {
	if len(v1) < 32 {
		var r float64
		for i := range v1 {
			r += float64(v1[i]) * float64(v2[i])
		}
		return r
	}
	m := len(v1) / 2
	return dotProd(v1[:m], v2[:m]) + dotProd(v1[m:], v2[m:])
}

func Cosine(v1, v2 []float32) (float64, error) {
	if len(v1) != len(v2) || len(v1) == 0 {
		return 0, fmt.Errorf("invalid arguments")
	}
	magnitude := dotProd(v1, v1) * dotProd(v2, v2)
	if magnitude == 0 {
		return 0, nil
	}
	p := dotProd(v1, v2)
	if p < 0 {
		return 0, nil
	}
	cos := p / math.Sqrt(magnitude)
	return cos, nil
}
```

Then I noticed that instead of tests for the cosine function, there was a benchmark ! So, I run it ...

## When the fix is faster ... ##

Yes, the recursive version of dot product was faster ... I didn't expected it.

This is what I got with go benchmarking tools:

```
BenchmarkCosine-8		1331042	       884.9 ns/op
BenchmarkCosineOld-8    853687	      1405 ns/op
```

Before going further, I tested it against a series naive tests and was happy to see that this version returns `1` when call on identical vectors.

So, I looked back at the original code, and finally noticed the first error.

First we compute the dot product that way:
```golang
	for k, x := range u {
		dotProduct += float64(x) * float64(v[k])
	}
```

Then the norm, that way:
```golang
	for _, x := range u {
		sumU += float64(x * x)
	}
```

Do you see it?

The conversion to `float64` is not done the same way on both loops and this is enough to have `Cosine(v,v)` returning a value different from `1.0`.

The second error is more difficult to cath, in fact this is more an approximation issue than an error by itself. I fixed this one, again, without knowing I was doing so. I just thought that I could compute the square root outside of the product, just to have one call. The approximation introduced by the square root, was enough to make results diverge a bit from the expected values.

## Going further ##

Happy with my finding, I wrote a PR. And one of the reviewer shows up with another faster version, again using the divide and conquer implementation.

```golang
func dotMergeProd(u, v []float32) (dotProduct, sumU, sumV float64) {
	if len(u) < 32 {
		for k, x := range u {
			dotProduct += float64(x) * float64(v[k])
			sumU += float64(x) * float64(x)
			sumV += float64(v[k]) * float64(v[k])
		}
		return dotProduct, sumU, sumV
	}
	n := len(u) / 2
	dotProduct, sumU, sumV = dotMergeProd(u[:n], v[:n])
	x, y, z := dotMergeProd(u[n:], v[n:])
	return dotProduct + x, sumU + y, sumV + z
}

func Cosine(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, errors.New("invalid arguments")
	}
	dotProduct, sumU, sumV := dotMergeProd(u, v)
	magnitude := math.Sqrt(sumU * sumV)
	if magnitude == 0 {
		return 0, nil
	}
	cos := dotProduct / magnitude
	if cos < 0 {
		return 0, nil
	}
	return cos, nil
}
```

Digging a bit further, the bound that control when to switch from recursion to loop seems to have a large impact on the performances. I am testing using vectors of 512 elements, since we divide by two the length, we cross the boundaries when the length is `16` (we test for strictly less than `32`). Replacing the bound so we enter the loop when the length is `32` gives the best results in our case. Doing the math, we have a recursion depth of `5` and then we're doing `16` loops on `32` elements.

Building on this idea, I _derecursified_ the code and get this version:

```golang
func dotProdBloc32(u, v []float32) (dp float64, sumU float64, sumV float64) {
	bound := min(len(u), 32)
	for i := 0; i < bound; i++ {
		ui, vi := float64(u[i]), float64(v[i])
		dp += ui * vi
		sumU += ui * ui
		sumV += vi * vi
	}
	return dp, sumU, sumV
}

func dotProdBloc(u, v []float32) (dp float64, sumU float64, sumV float64) {
	for offset := 0; offset < len(u); offset += 32 {
		end := min(offset+32, len(u))
		x, y, z := dotProdBloc32(u[offset:end], v[offset:end])
		dp += x
		sumU += y
		sumV += z
	}
	return
}

func CosineBlock(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, errors.New("invalid arguments")
	}
	dotProduct, sumU, sumV := dotProdBloc(u, v)
	magnitude := math.Sqrt(sumU * sumV)
	if magnitude == 0 {
		return 0, nil
	}
	cos := dotProduct / magnitude
	if cos < 0 {
		return 0, nil
	}
	return cos, nil
}
```

Just to be sure this was not due to some Go specificities, I translated both implementations in C++:

```c++
double cosine_simple_loop(auto&& u, auto&& v)
{
    if (u.size() != v.size()) {
        throw std::invalid_argument("not the same size");
    }
    double dp { 0 };
    for (size_t i = 0; i != u.size(); ++i) {
        dp += u[i] * v[i];
    }
    double norm2_u { 0 };
    for (auto&& x : u) {
        norm2_u += x * x;
    }
    double norm2_v { 0 };
    for (auto&& x : v) {
        norm2_v += x * x;
    }
    double magnitude = std::sqrt(norm2_u * norm2_v);
    if (magnitude == 0) {
        return 0;
    }
    double cos = dp / magnitude;
    if (cos < 0) {
        return 0;
    }
    return cos;
}

std::tuple<double, double, double> merged_dot_prod(auto uleft, auto uright, auto vleft)
{
    if (uright - uleft < 33) {
        double dp { 0 };
        double norm2_u { 0 };
        double norm2_v { 0 };
        for (; uleft != uright; ++uleft, ++vleft) {
            dp += *uleft * *vleft;
            norm2_u += *uleft * *uleft;
            norm2_v += *vleft * *vleft;
        }
        return { dp, norm2_u, norm2_v };
    }
    auto len                         = (uright - uleft) / 2;
    auto&& [dp1, norm2_u1, norm2_v1] = merged_dot_prod(uleft, uleft + len, vleft);
    auto&& [dp2, norm2_u2, norm2_v2] = merged_dot_prod(uleft + len, uright, vleft + len);
    return { dp1 + dp2, norm2_u1 + norm2_u2, norm2_v1 + norm2_v2 };
}

double cosine_grouped_dnc(auto&& u, auto&& v)
{
    if (u.size() != v.size()) {
        throw std::invalid_argument("not the same size");
    }
    auto&& [dp, norm2_u, norm2_v] = merged_dot_prod(begin(u), end(u), begin(v));
    if (dp < 0) {
        return 0;
    }
    double magnitude = std::sqrt(norm2_u * norm2_v);
    if (magnitude == 0) {
        return 0;
    }
    return dp / magnitude;
}
```

Timing are similar to the ones for the Go version:

| Implementation 	| Go			| C++   		|
| :---           	| :---:			| :---: 		|
| Simple loop		| 1275 ns/op	| 1125 ns/op	|
| Grouped DnC		| 345 ns/op		| 323 ns/op		|
| By blocks			| 319 ns/op 	| 296 ns/op		|

_Note: all benchmarks are run on my M1 laptop._
