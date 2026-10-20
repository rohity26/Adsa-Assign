// Q7: Minimum spanning tree that must contain a given set of K mandatory roads.
// 1) Union all mandatory roads first. If one of them joins two cities that are already connected,
//    the mandatory roads contain a cycle, so no spanning TREE can contain them all -> -1.
// 2) Run Kruskal on the remaining roads in increasing weight order (cut property: among trees that
//    contain the mandatory forest, greedily adding the lightest safe edge is optimal).
// 3) If the result does not connect all N cities, the graph has no spanning tree -> -1.
// Time: O(M log(M+1)) for sorting + O((N+M) alpha(N)) for the union-find operations.
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
typedef long long ll;

struct Edge { int u, v; ll w; };
bool byWeight(const Edge &a, const Edge &b) { return a.w < b.w; }

vector<int> parent, setSize;
int findRoot(int x) {
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
    int N, M, K; if (!(cin >> N >> M >> K)) return 0;
    vector<Edge> roads(M);
    for (int i = 0; i < M; i++) cin >> roads[i].u >> roads[i].v >> roads[i].w;
    vector<char> mandatory(M, 0);
    for (int i = 0; i < K; i++) { int idx; cin >> idx; mandatory[idx - 1] = 1; }   // 1-based indices

    parent.resize(N + 1); setSize.assign(N + 1, 1);
    for (int i = 0; i <= N; i++) parent[i] = i;
    ll total = 0; int edgesUsed = 0;
    for (int i = 0; i < M; i++) if (mandatory[i]) {
        if (!unite(roads[i].u, roads[i].v)) { cout << -1 << '\n'; return 0; }     // mandatory cycle
        total += roads[i].w; edgesUsed++;
    }
    vector<Edge> optional;
    for (int i = 0; i < M; i++) if (!mandatory[i]) optional.push_back(roads[i]);
    sort(optional.begin(), optional.end(), byWeight);
    for (size_t i = 0; i < optional.size() && edgesUsed < N - 1; i++)
        if (unite(optional[i].u, optional[i].v)) { total += optional[i].w; edgesUsed++; }
    if (edgesUsed != N - 1) cout << -1 << '\n';                                   // graph not connected
    else cout << total << '\n';
    return 0;
}
