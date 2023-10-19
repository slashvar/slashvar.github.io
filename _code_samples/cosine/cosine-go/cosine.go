package cosine

import (
	"errors"
	"fmt"
	"math"
)

// dotProd computes dot product using Divide and Conquer approach to minimize float approximation
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

func dotProdNaive(v1, v2 []float32) float64 {
	var r float64
	for i := range v1 {
		r += float64(v1[i]) * float64(v2[i])
	}
	return r
}

// Cosine computes the cosine angle between two vectors.
// This version is using divide and conquer algorithm to compute dot product.
func Cosine(v1, v2 []float32) (float64, error) {
	if len(v1) != len(v2) || len(v1) == 0 {
		return 0, fmt.Errorf("cannot cosine vectors with different lengths")
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

// CosineNaive computes the cosine angle between two vectors.
// This version is using divide and conquer algorithm to compute dot product.
func CosineNaive(v1, v2 []float32) (float64, error) {
	if len(v1) != len(v2) || len(v1) == 0 {
		return 0, fmt.Errorf("cannot cosine vectors with different lengths")
	}
	magnitude := dotProdNaive(v1, v1) * dotProdNaive(v2, v2)
	if magnitude == 0 {
		return 0, nil
	}
	p := dotProdNaive(v1, v2)
	if p < 0 {
		return 0, nil
	}
	cos := p / math.Sqrt(magnitude)
	return cos, nil
}

func CosineLoopMerge(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, fmt.Errorf("cannot cosine vectors with different lengths")
	}
	var dotProduct, sumU, sumV float64
	for k, x := range u {
		dotProduct += float64(x) * float64(v[k])
		sumU += float64(x) * float64(x)
		sumV += float64(v[k]) * float64(v[k])
	}
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

func dotMergeProd(u, v []float32) (dotProduct, sumU, sumV float64) {
	if len(u) < 64 {
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

func CosineSplitProd(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, errors.New("cannot cosine vectors with different lengths")
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

func CosineOneFunc(u, v []float32) (float64, error) {
	if len(u) != len(v) {
		return 0, errors.New("cannot cosine vectors with different lengths")
	}
	var dotProduct float64
	for k, x := range u {
		dotProduct += float64(x * v[k])
	}
	sumU := 0.0
	for _, x := range u {
		sumU += float64(x * x)
	}
	sumV := 0.0
	for _, x := range v {
		sumV += float64(x * x)
	}
	magnitude := math.Sqrt(sumU * sumV)
	if magnitude == 0 {
		return 0.0, nil
	}
	cos := dotProduct / magnitude
	if cos < 0 {
		return 0, nil
	}
	return cos, nil
}

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
		return 0, errors.New("cannot cosine vectors with different lengths")
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
