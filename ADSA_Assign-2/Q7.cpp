// Q7 : Mandatory roads in a minimum spanning tree.
//
// Problem: N cities, M possible roads (each with a cost), and K of these roads are MANDATORY.
// Find the cheapest spanning tree (connects all cities, no cycles) that contains every mandatory road,
// or report -1 if no such tree exists.
//
// Idea (Kruskal's algorithm with a head start):
//   1. First take ALL mandatory roads. If one of them joins two cities that are already connected, the
//      mandatory roads contain a cycle, and no tree can contain a cycle  -> answer -1.
//   2. Then run normal Kruskal on the other roads: look at them from cheapest to most expensive and take
//      a road only if it joins two cities that are not connected yet.
//   3. If at the end we do not have exactly N-1 roads, the graph cannot be connected -> answer -1.
// A disjoint set union (DSU) tells us in almost constant time whether two cities are already connected.
// Time: sorting O(M log M) plus DSU work O((N + M) * alpha(N)).
//
// Example from the assignment: N=5, M=7, mandatory roads number 1 and 3.
//   road 1: 1-2 (4)   road 2: 1-3 (2)   road 3: 2-3 (1)   road 4: 2-4 (5)
//   road 5: 3-4 (3)   road 6: 4-5 (2)   road 7: 3-5 (6)
//   step 1: take road 1 (4) and road 3 (1): cities 1, 2, 3 are connected, cost so far 5
//   step 2: remaining roads by cost: 2 (1-3), 6 (4-5), 5 (3-4), 4, 7
//           road 2 (1-3): 1 and 3 are already connected -> skip
//           road 6 (4-5): new -> take (cost 2)       road 5 (3-4): new -> take (cost 3)
//   we now have 4 = N-1 roads, total 4 + 1 + 2 + 3 = 10

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Road {
    int u;
    int v;
    long long cost;
};

// Comparison used by sort(): the road with the smaller cost comes first.
bool cheaperFirst(const Road &a, const Road &b) {
    return a.cost < b.cost;
}

// ---- Disjoint set union (see Q6 for the same structure) ----
vector<int> groupParent;
vector<int> groupSize;

// Leader of x's group, with path compression.
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

// Join the groups of a and b (smaller below bigger). Returns false if they were already together.
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

    int N, M, K;
    if (!(cin >> N >> M >> K)) {
        return 0;
    }

    // read all roads; road number i + 1 is stored at index i
    vector<Road> roads(M);
    for (int i = 0; i < M; i++) {
        cin >> roads[i].u >> roads[i].v >> roads[i].cost;
    }
    // mark the mandatory roads (when K = 0 there is no last line to read)
    vector<char> isMandatory(M, 0);
    for (int i = 0; i < K; i++) {
        int roadNumber;
        cin >> roadNumber;                      // roads are numbered from 1 in the input
        isMandatory[roadNumber - 1] = 1;
    }

    // at the start every city is a group of its own
    groupParent.resize(N + 1);
    groupSize.assign(N + 1, 1);
    for (int i = 0; i <= N; i++) {
        groupParent[i] = i;
    }

    long long totalCost = 0;
    int roadsTaken = 0;

    // step 1: the mandatory roads
    for (int i = 0; i < M; i++) {
        if (isMandatory[i]) {
            if (!joinGroups(roads[i].u, roads[i].v)) {
                cout << -1 << '\n';             // two mandatory roads close a cycle
                return 0;
            }
            totalCost += roads[i].cost;
            roadsTaken++;
        }
    }

    // step 2: the other roads, cheapest first. (We can stop as soon as N-1 roads are chosen,
    // because a tree on N cities has exactly N-1 roads.)
    vector<Road> optionalRoads;
    for (int i = 0; i < M; i++) {
        if (!isMandatory[i]) {
            optionalRoads.push_back(roads[i]);
        }
    }
    sort(optionalRoads.begin(), optionalRoads.end(), cheaperFirst);

    for (size_t i = 0; i < optionalRoads.size() && roadsTaken < N - 1; i++) {
        if (joinGroups(optionalRoads[i].u, optionalRoads[i].v)) {
            totalCost += optionalRoads[i].cost;
            roadsTaken++;
        }
    }

    // step 3: a spanning tree has exactly N-1 roads
    if (roadsTaken != N - 1) {
        cout << -1 << '\n';                     // the cities cannot all be connected
    } else {
        cout << totalCost << '\n';
    }
    return 0;
}
