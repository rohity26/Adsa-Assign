// Q9(a): Polynomial multiplication C(x) = A(x) * B(x) with the Fast Fourier Transform.
// 1) Pad both coefficient arrays to length L = smallest power of two >= n+m-1.
// 2) Forward FFT both (evaluate A and B at the L complex L-th roots of unity), O(L log L).
// 3) Multiply the values pointwise: C(w^k) = A(w^k) * B(w^k), O(L).
// 4) Inverse FFT (interpolate) to get the coefficients of C, then round each to the nearest
//    integer (the true coefficients are integers; FFT only adds tiny floating-point noise).
// With |a_i|, |b_j| <= 100 and n, m <= 2e5, |c_k| <= 2e9, far inside double precision.
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
using namespace std;
typedef complex<double> cd;
typedef long long ll;

// Iterative radix-2 FFT. invert = true computes the inverse transform (including the 1/L scaling).
void fft(vector<cd> &a, bool invert, const vector<cd> &roots) {
    int L = a.size();
    for (int i = 1, j = 0; i < L; i++) {                 // bit-reversal permutation
        int bit = L >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }
    for (int len = 2; len <= L; len <<= 1) {             // butterflies, block size len
        int step = L / len;                               // roots[k*step] = exp(2*pi*i*k/len)
        for (int i = 0; i < L; i += len)
            for (int k = 0; k < len / 2; k++) {
                cd w = roots[k * step];
                if (invert) w = conj(w);
                cd u = a[i + k], v = a[i + k + len / 2] * w;
                a[i + k] = u + v;
                a[i + k + len / 2] = u - v;
            }
    }
    if (invert) for (int i = 0; i < L; i++) a[i] /= L;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    int n, m; if (!(cin >> n >> m)) return 0;
    int need = n + m - 1, L = 1;
    while (L < need) L <<= 1;
    vector<cd> fa(L, cd(0, 0)), fb(L, cd(0, 0));
    for (int i = 0; i < n; i++) { int x; cin >> x; fa[i] = cd(x, 0); }
    for (int j = 0; j < m; j++) { int x; cin >> x; fb[j] = cd(x, 0); }
    // roots of unity computed directly with cos/sin (not by repeated multiplication) for accuracy
    const double PI = acos(-1.0);
    vector<cd> roots(L / 2 > 0 ? L / 2 : 1);
    for (int k = 0; k < (int)roots.size(); k++) roots[k] = cd(cos(2 * PI * k / L), sin(2 * PI * k / L));
    fft(fa, false, roots);
    fft(fb, false, roots);
    for (int i = 0; i < L; i++) fa[i] *= fb[i];
    fft(fa, true, roots);
    for (int k = 0; k < need; k++) {
        ll c = (ll)floor(fa[k].real() + 0.5);             // round to the nearest integer
        if (k) cout << ' ';
        cout << c;
    }
    cout << '\n';
    return 0;
}
