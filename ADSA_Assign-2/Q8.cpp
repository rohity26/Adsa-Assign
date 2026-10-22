// Q8 : Circular pattern matching with Knuth-Morris-Pratt (KMP).
//
// Problem: S is a circular string of length N (after S[N-1] comes S[0] again) and P is a pattern of length M.
// Report every start position i (0 <= i < N) where the M characters read circularly from i equal P.
// Overlapping matches and matches that wrap around the end of S both count.
//
// Idea 1 - remove the circle: a circular window that starts at i reads S[i], S[i+1], ... and may run past
// the end back to S[0]. If we glue the first M-1 characters of S behind S, i.e.
//        text = S + S[0 .. M-2]            (length N + M - 1)
// every circular window becomes an ordinary window of 'text'. Windows that start at i >= N are not needed,
// and 'text' contains no window starting at i >= N anyway (it would be too short), so every match we find
// has a start position < N automatically.
//
// Idea 2 - KMP finds all matches of P in 'text' in O(length) time. It never moves backwards in the text:
// when a character does not match, the "LPS array" tells how much of the pattern we can still keep.
//
// LPS array: lps[i] = length of the longest proper prefix of P[0..i] that is also a suffix of P[0..i].
// Example P = "abab":   i=0 "a"    -> 0
//                       i=1 "ab"   -> 0   (no prefix equals a suffix)
//                       i=2 "aba"  -> 1   ("a" is both prefix and suffix)
//                       i=3 "abab" -> 2   ("ab" is both prefix and suffix)           lps = [0, 0, 1, 2]
//
// Example from the assignment: S = "abab", P = "bab" (N=4, M=3).
//   M-1 = 2 characters are added behind S:  text = "abab" + "ab" = "ababab"
//   "bab" occurs in "ababab" at positions 1 and 3  ->  answer: 2 matches, starting at 1 and 3
//   (position 3 is the wrap-around window S[3], S[0], S[1] = b, a, b)

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Build the LPS array of the pattern in O(M).
vector<int> buildLPS(const string &pattern) {
    int M = pattern.size();
    vector<int> lps(M, 0);
    int borderLength = 0;                          // length of the border of the part seen so far
    int i = 1;
    while (i < M) {
        if (pattern[i] == pattern[borderLength]) {
            // the border can be extended by one character
            borderLength++;
            lps[i] = borderLength;
            i++;
        } else if (borderLength > 0) {
            // try a shorter border (the border of the border); do not move i
            borderLength = lps[borderLength - 1];
        } else {
            lps[i] = 0;
            i++;
        }
    }
    return lps;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;
    if (!(cin >> N >> M)) {
        return 0;
    }
    string S, P;
    cin >> S >> P;

    string text = S + S.substr(0, M - 1);           // unroll the circle just far enough
    vector<int> lps = buildLPS(P);
    vector<int> matches;

    int matched = 0;                                // how many pattern characters match right now
    for (int i = 0; i < (int)text.size(); i++) {
        // on a mismatch fall back to the longest shorter prefix that still matches
        while (matched > 0 && text[i] != P[matched]) {
            matched = lps[matched - 1];
        }
        if (text[i] == P[matched]) {
            matched++;
        }
        if (matched == M) {
            matches.push_back(i - M + 1);           // a full match ends at i
            matched = lps[matched - 1];             // keep going: overlapping matches are allowed
        }
    }

    cout << matches.size() << '\n';
    if (!matches.empty()) {
        for (size_t k = 0; k < matches.size(); k++) {
            if (k > 0) {
                cout << ' ';
            }
            cout << matches[k];
        }
        cout << '\n';
    }
    return 0;
}
