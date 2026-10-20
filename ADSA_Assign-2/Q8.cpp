// Q8: Circular pattern matching with Knuth-Morris-Pratt.
// A circular match starting at i (0 <= i < N) reads S[i], S[i+1], ... wrapping past S[N-1] to S[0].
// Such a window lies entirely inside the linear string  text = S + S[0 .. M-2]  (length N+M-1),
// and every window of text that starts at i < N is exactly one circular window. So we run ordinary
// KMP on text; every match start is automatically < N. Total time O(N + M).
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// lps[i] = length of the longest proper prefix of P[0..i] that is also a suffix of P[0..i]
vector<int> buildLPS(const string &P) {
    int M = P.size();
    vector<int> lps(M, 0);
    int len = 0;                                   // length of the current border
    for (int i = 1; i < M; ) {
        if (P[i] == P[len]) lps[i++] = ++len;
        else if (len > 0) len = lps[len - 1];       // fall back to the next shorter border
        else lps[i++] = 0;
    }
    return lps;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    int N, M; if (!(cin >> N >> M)) return 0;
    string S, P; cin >> S >> P;
    string text = S + S.substr(0, M - 1);          // unroll the circle just enough for wrap-around
    vector<int> lps = buildLPS(P), matches;
    int j = 0;                                     // number of pattern characters currently matched
    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && text[i] != P[j]) j = lps[j - 1];   // text pointer i never moves back
        if (text[i] == P[j]) j++;
        if (j == M) { matches.push_back(i - M + 1); j = lps[j - 1]; }   // keep going: overlaps allowed
    }
    cout << matches.size() << '\n';
    if (!matches.empty()) {
        for (size_t k = 0; k < matches.size(); k++) { if (k) cout << ' '; cout << matches[k]; }
        cout << '\n';
    }
    return 0;
}
