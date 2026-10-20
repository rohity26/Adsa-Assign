// Q5: Shortest path S -> T where exactly one road (u0 -> v0, weight w0 < 0) is negative
// and may be used at most once.
// Dijkstra is never run on a graph containing the negative road. Instead, on the graph WITHOUT it:
//   distS[v] = shortest distance S -> v      (Dijkstra from S)
//   distV[v] = shortest distance v0 -> v     (Dijkstra from v0)
// Any optimal walk either avoids the negative road (cost distS[T]) or uses it exactly once
// (cost distS[u0] + w0 + distV[T]). The answer is the smaller of the two.
#include <iostream>
#include <vector>
#include <queue>
using namespace std;
typedef long long ll;
const ll INF = (ll)4e18;

struct Edge { int to; ll w; };

void dijkstra(int src, const vector<vector<Edge> > &adj, vector<ll> &dist) {
    dist.assign(adj.size(), INF);
    priority_queue<pair<ll, int>, vector<pair<ll, int> >, greater<pair<ll, int> > > pq;
    dist[src] = 0; pq.push(make_pair(0LL, src));
    while (!pq.empty()) {
        ll d = pq.top().first; int u = pq.top().second; pq.pop();
        if (d > dist[u]) continue;                 // stale queue entry
        for (size_t i = 0; i < adj[u].size(); i++) {
            int v = adj[u][i].to; ll nd = d + adj[u][i].w;
            if (nd < dist[v]) { dist[v] = nd; pq.push(make_pair(nd, v)); }
        }
    }
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    int N, M, S, T;
    if (!(cin >> N >> M >> S >> T)) return 0;
    vector<vector<Edge> > adj(N + 1);
    int negFrom = -1, negTo = -1; ll negW = 0;
    for (int i = 0; i < M; i++) {
        int a, b; ll c; cin >> a >> b >> c;
        if (c < 0) { negFrom = a; negTo = b; negW = c; }   // keep the negative road aside
        else { Edge e; e.to = b; e.w = c; adj[a].push_back(e); }
    }
    vector<ll> distS, distV;
    dijkstra(S, adj, distS);
    ll best = distS[T];                                    // option 1: never use the negative road
    if (negFrom != -1 && distS[negFrom] < INF) {
        dijkstra(negTo, adj, distV);
        if (distV[T] < INF) {                              // option 2: S ~> u0 -> v0 ~> T
            ll viaNeg = distS[negFrom] + negW + distV[T];
            if (viaNeg < best) best = viaNeg;
        }
    }
    if (best >= INF) cout << "UNREACHABLE\n";
    else cout << best << '\n';
    return 0;
}
