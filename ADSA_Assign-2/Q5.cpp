// Q5 : The forbidden shortcut.
//
// Problem: shortest travel time from city S to city T in a directed graph where EXACTLY ONE road has a
// negative time and every other road is non-negative. The negative road may be used at most once.
// Dijkstra's algorithm is only correct for non-negative weights, so it must never see the negative road.
//
// Idea: remove the negative road u0 -> v0 (weight w0 < 0) and run Dijkstra on the remaining graph.
//   distS[x] = shortest time from S to x
//   distV[x] = shortest time from v0 to x
// A best trip either
//   (1) never uses the negative road:           cost  distS[T]
//   (2) uses it exactly once, so it looks like  S ~~> u0 -> v0 ~~> T :   cost  distS[u0] + w0 + distV[T]
// The answer is the smaller of the two (or UNREACHABLE if neither trip exists).
//
// Example from the assignment: N=6, S=1, T=6, roads
//    1->2 (4)  2->3 (3)  3->6 (5)  1->4 (2)  4->5 (2)  5->6 (10)  and the negative road 2->5 (-8)
//   without the negative road: 1->2->3->6 = 12   (1->4->5->6 = 14)          option (1) = 12
//   with it: distS[2] = 4, w0 = -8, distV[6] from v0=5 is 10                option (2) = 4 - 8 + 10 = 6
//   answer = min(12, 6) = 6
//
// All costs are 64-bit (a path can have up to 2*10^5 roads of cost up to 10^9).

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const long long INF = 4000000000000000000LL;      // "no path" marker (bigger than any real cost)

struct Road {
    int to;
    long long time;
};

// Standard Dijkstra with a priority queue (smallest distance first).
// dist[x] ends up as the shortest time from 'source' to x, or INF if x cannot be reached.
void dijkstra(int source, const vector<vector<Road> > &graph, vector<long long> &dist) {
    dist.assign(graph.size(), INF);
    priority_queue<pair<long long, int>, vector<pair<long long, int> >, greater<pair<long long, int> > > queue;

    dist[source] = 0;
    queue.push(make_pair(0LL, source));

    while (!queue.empty()) {
        long long distanceSoFar = queue.top().first;
        int city = queue.top().second;
        queue.pop();

        if (distanceSoFar > dist[city]) {
            continue;                              // an older, longer entry for this city: skip it
        }
        for (size_t i = 0; i < graph[city].size(); i++) {
            int next = graph[city][i].to;
            long long newDistance = distanceSoFar + graph[city][i].time;
            if (newDistance < dist[next]) {
                dist[next] = newDistance;
                queue.push(make_pair(newDistance, next));
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M, S, T;
    if (!(cin >> N >> M >> S >> T)) {
        return 0;
    }

    vector<vector<Road> > graph(N + 1);            // cities are numbered 1..N
    int negativeFrom = -1;                         // u0
    int negativeTo = -1;                           // v0
    long long negativeTime = 0;                    // w0
    for (int i = 0; i < M; i++) {
        int a, b;
        long long c;
        cin >> a >> b >> c;
        if (c < 0) {
            // keep the negative road aside, it is NOT added to the graph that Dijkstra sees
            negativeFrom = a;
            negativeTo = b;
            negativeTime = c;
        } else {
            Road road;
            road.to = b;
            road.time = c;
            graph[a].push_back(road);
        }
    }

    vector<long long> distFromS;
    dijkstra(S, graph, distFromS);

    long long best = distFromS[T];                 // option (1): do not use the negative road

    // option (2): only possible if we can reach u0 from S ...
    if (negativeFrom != -1 && distFromS[negativeFrom] < INF) {
        vector<long long> distFromV;
        dijkstra(negativeTo, graph, distFromV);
        // ... and can reach T from v0
        if (distFromV[T] < INF) {
            long long throughNegative = distFromS[negativeFrom] + negativeTime + distFromV[T];
            if (throughNegative < best) {
                best = throughNegative;
            }
        }
    }

    if (best >= INF) {
        cout << "UNREACHABLE\n";
    } else {
        cout << best << '\n';
    }
    return 0;
}
