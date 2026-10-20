// Q6: Minimum cost to electrify all cities.
// Add a virtual node 0 ("the power grid") with an edge 0 - i of cost P_i for every city i:
// building a station in city i  <=>  using edge (0, i). A set of stations + lines electrifies every
// city exactly when every city is connected to node 0, so the answer is the MST weight of the
// graph on N+1 nodes. Computed with Kruskal + disjoint-set union (path compression + union by size).
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
typedef long long ll;

struct Edge { int u, v; ll w; };
bool byWeight(const Edge &a, const Edge &b) { return a.w < b.w; }

vector<int> parent, setSize;
int findRoot(int x) {                       // iterative find with path compression
    int r = x; while (parent[r] != r) r = parent[r];
    while (parent[x] != r) { int next = parent[x]; parent[x] = r; x = next; }
    return r;
}
bool unite(int a, int b) {
    a = findRoot(a); b = findRoot(b);
    if (a == b) return false;
    if (setSize[a] < setSize[b]) swap(a, b);
    parent[b] = a; setSize[a] += setSize[b];
    return true;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    int N, M; if (!(cin >> N >> M)) return 0;
    vector<Edge> edges; edges.reserve(N + M);
    for (int i = 1; i <= N; i++) { Edge e; e.u = 0; e.v = i; cin >> e.w; edges.push_back(e); }   // station edges
    for (int i = 0; i < M; i++) { Edge e; cin >> e.u >> e.v >> e.w; edges.push_back(e); }       // power lines
    sort(edges.begin(), edges.end(), byWeight);
    parent.resize(N + 1); setSize.assign(N + 1, 1);
    for (int i = 0; i <= N; i++) parent[i] = i;
    ll total = 0; int used = 0;
    for (size_t i = 0; i < edges.size() && used < N; i++)
        if (unite(edges[i].u, edges[i].v)) { total += edges[i].w; used++; }
    cout << total << '\n';
    return 0;
}
