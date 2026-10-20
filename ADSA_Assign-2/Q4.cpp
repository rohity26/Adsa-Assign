// Q4: 2-CNF satisfiability via the implication graph + strongly connected components (Tarjan).
// Literal encoding: variable i (0-based) -> node 2i+1 means x_i, node 2i means NOT x_i; negate = node ^ 1.
// Clause (a OR b) gives implications (NOT a -> b) and (NOT b -> a).
// Unsatisfiable  <=>  some x_i and NOT x_i lie in the same SCC.
// Tarjan numbers SCCs in reverse topological order, so x_i = true iff comp[x_i] < comp[NOT x_i]
// (x_i is node 2i+1 here, NOT x_i is node 2i).
#include <iostream>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    int t; if (!(cin >> t)) return 0;
    while (t--) {
        int n, m; cin >> n >> m;
        int V = 2 * n;
        vector<int> from(2 * m), to(2 * m);
        for (int i = 0; i < m; i++) {
            int a, b; cin >> a >> b;
            int la = (a > 0) ? 2 * (a - 1) + 1 : 2 * (-a - 1);
            int lb = (b > 0) ? 2 * (b - 1) + 1 : 2 * (-b - 1);
            from[2 * i] = la ^ 1; to[2 * i] = lb;          // NOT a -> b
            from[2 * i + 1] = lb ^ 1; to[2 * i + 1] = la;  // NOT b -> a
        }
        // adjacency in compressed (CSR) form
        vector<int> start(V + 1, 0), adj(2 * m);
        for (int e = 0; e < 2 * m; e++) start[from[e] + 1]++;
        for (int v = 0; v < V; v++) start[v + 1] += start[v];
        vector<int> fillPos(start.begin(), start.end() - 1);
        for (int e = 0; e < 2 * m; e++) adj[fillPos[from[e]]++] = to[e];

        // iterative Tarjan
        vector<int> index(V, -1), low(V, 0), comp(V, -1), edgePos(V, 0), sccStack, callStack;
        vector<char> onStack(V, 0);
        int counter = 0, compCount = 0;
        for (int s = V - 1; s >= 0; s--) {                  // DFS roots in decreasing node order
            if (index[s] != -1) continue;
            callStack.push_back(s);
            index[s] = low[s] = counter++; edgePos[s] = start[s];
            sccStack.push_back(s); onStack[s] = 1;
            while (!callStack.empty()) {
                int v = callStack.back();
                if (edgePos[v] < start[v + 1]) {
                    int w = adj[edgePos[v]++];
                    if (index[w] == -1) {                       // tree edge: "recurse" into w
                        index[w] = low[w] = counter++; edgePos[w] = start[w];
                        sccStack.push_back(w); onStack[w] = 1; callStack.push_back(w);
                    } else if (onStack[w] && index[w] < low[v]) low[v] = index[w];
                } else {                                        // v is finished
                    callStack.pop_back();
                    if (!callStack.empty()) { int p = callStack.back(); if (low[v] < low[p]) low[p] = low[v]; }
                    if (low[v] == index[v]) {                   // v is the root of an SCC
                        int w;
                        do { w = sccStack.back(); sccStack.pop_back(); onStack[w] = 0; comp[w] = compCount; } while (w != v);
                        compCount++;
                    }
                }
            }
        }
        bool satisfiable = true;
        for (int i = 0; i < n; i++) if (comp[2 * i] == comp[2 * i + 1]) { satisfiable = false; break; }
        if (!satisfiable) { cout << "Unsatisfiable\n"; continue; }
        for (int i = 0; i < n; i++) { if (i) cout << ' '; cout << (comp[2 * i + 1] < comp[2 * i] ? 1 : 0); }
        cout << '\n';
    }
    return 0;
}
