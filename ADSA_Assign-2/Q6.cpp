// Q6 : Powering the cities.
//
// Problem: N cities, a power station in city i costs P[i], and some power lines between cities can be built
// (each with its own cost). A city has power if it has a station or is connected through built lines to a city
// with a station. Find the cheapest way to give power to every city.
//
// Idea: add ONE extra node 0 that stands for "the power source" and join it to every city i by an edge of
// cost P[i]. Building a station in city i  =  using the edge (0, i).
// A set of stations and lines gives power to every city exactly when every city is connected to node 0,
// so we need a cheapest connected graph on the N+1 nodes: a minimum spanning tree (MST).
// We build it with Kruskal's algorithm:
//   sort all edges by cost, and take an edge when it joins two groups that are not connected yet.
// A "disjoint set union" (DSU) answers "are these two cities already connected?" almost instantly.
//
// Example from the assignment: N=4, P = 5 2 7 6, lines 1-2 (4), 2-3 (3), 3-4 (2), 1-4 (8)
//   extra edges: 0-1 (5), 0-2 (2), 0-3 (7), 0-4 (6)
//   sorted: 3-4 (2), 0-2 (2), 2-3 (3), 1-2 (4), 0-1 (5), 0-4 (6), 0-3 (7), 1-4 (8)
//   take 3-4 (2), 0-2 (2), 2-3 (3), 1-2 (4): now all five nodes are connected -> total 2+2+3+4 = 11
//   (meaning: a station in city 2 only, plus the lines 2-3, 3-4 and 1-2)

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Edge {
    int u;
    int v;
    long long cost;
};

// Comparison used by sort(): the edge with the smaller cost comes first.
bool cheaperFirst(const Edge &a, const Edge &b) {
    return a.cost < b.cost;
}

// ---- Disjoint set union ----
// groupParent[x] points towards the "leader" of x's group; a leader points to itself.
// Example: groups {1,2} and {3}: groupParent = [.., 1, 1, 3]  (leader of 2 is 1)
vector<int> groupParent;
vector<int> groupSize;

// Find the leader of x. While walking up we make every visited node point directly to the leader
// ("path compression"), so later searches are very short.
int findLeader(int x) {
    int leader = x;
    while (groupParent[leader] != leader) {
        leader = groupParent[leader];
    }
    while (groupParent[x] != leader) {
        int next = groupParent[x];
        groupParent[x] = leader;
        x = next;
    }
    return leader;
}

// Join the groups of a and b. Returns false if they were already in the same group.
// The smaller group is attached below the bigger one ("union by size") to keep trees shallow.
bool joinGroups(int a, int b) {
    a = findLeader(a);
    b = findLeader(b);
    if (a == b) {
        return false;
    }
    if (groupSize[a] < groupSize[b]) {
        swap(a, b);
    }
    groupParent[b] = a;
    groupSize[a] += groupSize[b];
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;
    if (!(cin >> N >> M)) {
        return 0;
    }

    vector<Edge> edges;
    edges.reserve(N + M);

    // edge 0 - i  with cost P[i]: "build a station in city i"
    for (int i = 1; i <= N; i++) {
        Edge stationEdge;
        stationEdge.u = 0;
        stationEdge.v = i;
        cin >> stationEdge.cost;
        edges.push_back(stationEdge);
    }
    // the possible power lines
    for (int i = 0; i < M; i++) {
        Edge line;
        cin >> line.u >> line.v >> line.cost;
        edges.push_back(line);
    }

    sort(edges.begin(), edges.end(), cheaperFirst);

    groupParent.resize(N + 1);
    groupSize.assign(N + 1, 1);
    for (int i = 0; i <= N; i++) {
        groupParent[i] = i;                      // at the start every node is its own group
    }

    long long totalCost = 0;
    int edgesTaken = 0;
    for (size_t i = 0; i < edges.size() && edgesTaken < N; i++) {
        // N+1 nodes need N edges to be connected, so we can stop after N edges
        if (joinGroups(edges[i].u, edges[i].v)) {
            totalCost += edges[i].cost;
            edgesTaken++;
        }
    }
    cout << totalCost << '\n';
    return 0;
}
