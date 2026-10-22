// Q9 (a) : Multiply two polynomials with the Fast Fourier Transform (FFT).
//
// A(x) = a0 + a1*x + ... + a(n-1)*x^(n-1)      B(x) = b0 + b1*x + ... + b(m-1)*x^(m-1)
// C(x) = A(x) * B(x)  has n+m-1 coefficients, and  c_k = sum of a_i * b_j over all i + j = k.
// Doing that sum directly costs O(n*m). With the FFT it costs O(L log L).
//
// Idea: a polynomial of degree < L is completely described by its values at L different points
// ("value form"). In value form multiplying is trivial: C(point) = A(point) * B(point), one product per
// point. The FFT is a fast way (O(L log L)) to move from the coefficients to the values at the L-th roots of
// unity, and the inverse FFT moves back. So:
//   1. pad A and B with zeros to length L = smallest power of two >= n+m-1
//   2. FFT(A), FFT(B)                         -> values at the L roots of unity
//   3. multiply the values one by one         -> values of C
//   4. inverse FFT                            -> coefficients of C
//   5. round every result to the nearest whole number (the FFT works with floating point numbers, so
//      the results are only approximately integers, e.g. 27.0000000003)
//
// Example from the assignment: A = 1 + 2x + 3x^2, B = 4 + 5x + 6x^2  (n = m = 3, so n+m-1 = 5 and L = 8)
//   C = 4 + 13x + 28x^2 + 27x^3 + 18x^4          -> output "4 13 28 27 18"
//   check c2 = a0*b2 + a1*b1 + a2*b0 = 1*6 + 2*5 + 3*4 = 28
//
// The L-th roots of unity are the complex numbers w^k = cos(2*pi*k/L) + i*sin(2*pi*k/L), k = 0..L-1.
//
// Precision: |a_i|, |b_j| <= 100 and n, m <= 2*10^5, so every |c_k| <= 100*100*2*10^5 = 2*10^9. That is tiny
// compared with what a double can hold exactly, and the FFT error stays far below 0.5, so rounding is safe.
// (The roots of unity are computed directly with cos/sin, which is more accurate than multiplying them up.)

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
using namespace std;

typedef complex<double> Complex;

// In-place iterative FFT of 'values' (its length L is a power of two).
// invert == false : coefficients -> values at the roots of unity
// invert == true  : values -> coefficients (uses the conjugate roots and divides by L at the end)
// roots[k] = e^(2*pi*i*k/L) for k = 0 .. L/2 - 1
void fft(vector<Complex> &values, bool invert, const vector<Complex> &roots) {
    int L = values.size();

    // Step 1: reorder the entries by "bit reversal" so that the butterflies below can work in place.
    // Example L = 8: position 1 (001) swaps with 4 (100), 3 (011) with 6 (110); 0, 2, 5, 7 stay.
    for (int i = 1, j = 0; i < L; i++) {
        int bit = L >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) {
            swap(values[i], values[j]);
        }
    }

    // Step 2: combine blocks of size 2, then 4, then 8, ... up to L ("butterflies").
    // A block of size len is built from its two halves: with u = left entry and v = w * right entry,
    //     new left  = u + v        new right = u - v
    // where w is the matching root of unity.  Example for len = 2 (w = 1): [a, b] -> [a + b, a - b].
    for (int len = 2; len <= L; len <<= 1) {
        int step = L / len;                         // roots[k * step] = e^(2*pi*i*k/len)
        for (int start = 0; start < L; start += len) {
            for (int k = 0; k < len / 2; k++) {
                Complex w = roots[k * step];
                if (invert) {
                    w = conj(w);                    // the inverse transform uses the conjugate roots
                }
                Complex u = values[start + k];
                Complex v = values[start + k + len / 2] * w;
                values[start + k] = u + v;
                values[start + k + len / 2] = u - v;
            }
        }
    }

    // Step 3: the inverse transform must be divided by L to give back the original coefficients
    if (invert) {
        for (int i = 0; i < L; i++) {
            values[i] /= L;
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) {
        return 0;
    }
    int resultLength = n + m - 1;                   // number of coefficients of C
    int L = 1;
    while (L < resultLength) {
        L <<= 1;                                    // smallest power of two that is big enough
    }

    // read the coefficients; the entries beyond n (or m) stay 0 ("zero padding")
    vector<Complex> fa(L, Complex(0, 0));
    vector<Complex> fb(L, Complex(0, 0));
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        fa[i] = Complex(x, 0);
    }
    for (int j = 0; j < m; j++) {
        int x;
        cin >> x;
        fb[j] = Complex(x, 0);
    }

    // the roots of unity e^(2*pi*i*k/L), only half of them are needed
    const double PI = acos(-1.0);
    vector<Complex> roots(L / 2 > 0 ? L / 2 : 1);
    for (int k = 0; k < (int)roots.size(); k++) {
        roots[k] = Complex(cos(2 * PI * k / L), sin(2 * PI * k / L));
    }

    fft(fa, false, roots);                          // values of A
    fft(fb, false, roots);                          // values of B
    for (int i = 0; i < L; i++) {
        fa[i] = fa[i] * fb[i];                      // values of C = A * B
    }
    fft(fa, true, roots);                           // back to coefficients of C

    for (int k = 0; k < resultLength; k++) {
        long long coefficient = (long long)floor(fa[k].real() + 0.5);   // round to the nearest integer
        if (k > 0) {
            cout << ' ';
        }
        cout << coefficient;
    }
    cout << '\n';
    return 0;
}
