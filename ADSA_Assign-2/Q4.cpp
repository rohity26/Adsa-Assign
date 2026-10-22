// Q4 : 2-CNF satisfiability.
//
// Problem: given clauses with exactly two literals, e.g. (x1 OR NOT x2), decide whether some assignment of
// 0/1 to the variables makes every clause true, and if so print one.
//
// Idea (implication graph):
//   A clause (a OR b) means "if a is false then b is true" and "if b is false then a is true".
//   So every clause gives two arrows:   NOT a -> b    and    NOT b -> a.
//   The graph has one node per literal: x_i and NOT x_i, i.e. 2n nodes and 2m arrows.
//   An arrow p -> q means "if p is true, q is forced to be true".
//
//   If p can reach q and q can reach p they must have the same value; such a group of nodes is a
//   "strongly connected component". We find them with Tarjan's algorithm.
//     * If x_i and NOT x_i are in the same component the formula is UNSATISFIABLE.
//     * Otherwise a solution exists: for every variable make true the literal whose component id is SMALLER.
//       (Tarjan numbers a component only after everything it points to is numbered, so a smaller id means
//       "further downstream", and making the downstream literal true never contradicts an arrow.)
//
// Node numbers: variable number v (1-based in the input) uses node 2*(v-1) for NOT x_v and node 2*(v-1)+1
// for x_v. So  NOT x1 = 0, x1 = 1, NOT x2 = 2, x2 = 3, NOT x3 = 4, x3 = 5 ...
// The negation of a node is found by flipping its lowest bit: 0 <-> 1, 2 <-> 3, 4 <-> 5 (node ^ 1).
//
// Example 1 from the assignment: clauses (x1 OR -x2), (-x1 OR -x3), (-x1 OR x2)
//   arrows       : -x1 -> -x2   x2 -> x1   |   x1 -> -x3   x3 -> -x1   |   x1 -> x2   -x2 -> -x1
//   components   : {-x1,-x2} gets id 0   {x3} id 1   {-x3} id 2   {x1,x2} id 3
//   x1: id(x1)=3, id(-x1)=0 -> 3 is not smaller -> 0       x2: same -> 0
//   x3: id(x3)=1, id(-x3)=2 -> 1 is smaller     -> 1       output: 0 0 1
// Example 2: all four clauses on x1,x2 -> all four literals in ONE component -> "Unsatisfiable".

#include <iostream>
#include <vector>
using namespace std;

// Node of an input literal: +k means x_k, -k means NOT x_k.
int nodeOfLiteral(int literal) {
    if (literal > 0) {
        return 2 * (literal - 1) + 1;
    }
    return 2 * (-literal - 1);
}

// Node of the negated literal (0<->1, 2<->3, ...).
int negationOf(int node) {
    return node ^ 1;
}

// Put the arrows (from[e] -> to[e]) into a compact array form (CSR):
// the nodes reachable from v by one arrow are  adj[ start[v] ... start[v+1]-1 ].
// Example: arrows 0->2, 0->3, 1->3  with 4 nodes give  start = [0, 2, 3, 3, 3]  and  adj = [2, 3, 3].
void buildAdjacency(int nodeCount, const vector<int> &from, const vector<int> &to,
                    vector<int> &start, vector<int> &adj) {
    int edgeCount = from.size();
    start.assign(nodeCount + 1, 0);
    adj.assign(edgeCount, 0);

    for (int e = 0; e < edgeCount; e++) {         // count the arrows leaving every node
        start[from[e] + 1]++;
    }
    for (int v = 0; v < nodeCount; v++) {         // prefix sums: start[v] = where the arrows of v begin
        start[v + 1] += start[v];
    }
    vector<int> nextFreeSlot(start.begin(), start.end() - 1);
    for (int e = 0; e < edgeCount; e++) {         // place every arrow into the block of its start node
        adj[nextFreeSlot[from[e]]] = to[e];
        nextFreeSlot[from[e]]++;
    }
}

// Tarjan's strongly-connected-components algorithm WITHOUT recursion (a chain of 200000 implications
// would overflow the call stack of a recursive version). Fills comp[v] with the component id of every node
// and returns the number of components.
//
//   index[v]  : order in which v was first visited (-1 = not visited yet)
//   low[v]    : smallest index reachable from v's search through tree edges plus one edge back to a node
//               that is still waiting on sccStack. v is the ROOT of its component when low[v] == index[v].
//   edgePos[v]: next arrow of v still to be examined (replaces the loop variable of a recursive version)
//   callStack : the nodes of the current DFS path (replaces the recursion)
//   sccStack  : visited nodes that are not yet assigned to a component
//   Component ids are handed out in the order components FINISH: 0, 1, 2, ...
int findComponents(int nodeCount, const vector<int> &start, const vector<int> &adj, vector<int> &comp) {
    vector<int> index(nodeCount, -1);
    vector<int> low(nodeCount, 0);
    vector<int> edgePos(nodeCount, 0);
    vector<int> sccStack;
    vector<int> callStack;
    vector<char> onSccStack(nodeCount, 0);
    comp.assign(nodeCount, -1);
    int visitCounter = 0;
    int componentCount = 0;

    // start a search from every node that has not been reached yet
    // (decreasing order only decides WHICH correct answer is printed; it reproduces the sample output)
    for (int s = nodeCount - 1; s >= 0; s--) {
        if (index[s] != -1) {
            continue;
        }
        // "call" the search on s
        callStack.push_back(s);
        index[s] = visitCounter;
        low[s] = visitCounter;
        visitCounter++;
        edgePos[s] = start[s];
        sccStack.push_back(s);
        onSccStack[s] = 1;

        while (!callStack.empty()) {
            int v = callStack.back();

            if (edgePos[v] < start[v + 1]) {
                // v still has an arrow v -> w that we have not looked at
                int w = adj[edgePos[v]];
                edgePos[v]++;
                if (index[w] == -1) {
                    // w is new: go down into it (this is the "recursive call")
                    index[w] = visitCounter;
                    low[w] = visitCounter;
                    visitCounter++;
                    edgePos[w] = start[w];
                    sccStack.push_back(w);
                    onSccStack[w] = 1;
                    callStack.push_back(w);
                } else if (onSccStack[w] && index[w] < low[v]) {
                    // w was seen before and is still unassigned, so it belongs to v's current component:
                    // v can get back to w's index. (If w is not on sccStack its component is finished: ignore.)
                    low[v] = index[w];
                }
            } else {
                // all arrows of v are processed: v is finished (the "return" of the recursive call)
                callStack.pop_back();
                if (!callStack.empty()) {
                    int parent = callStack.back();
                    if (low[v] < low[parent]) {
                        low[parent] = low[v];
                    }
                }
                if (low[v] == index[v]) {
                    // v is the root of a component: everything above v on sccStack belongs to it
                    int member;
                    do {
                        member = sccStack.back();
                        sccStack.pop_back();
                        onSccStack[member] = 0;
                        comp[member] = componentCount;
                    } while (member != v);
                    componentCount++;
                }
            }
        }
    }
    return componentCount;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int testCases;
    if (!(cin >> testCases)) {
        return 0;
    }
    while (testCases--) {
        int n, m;                                   // n variables, m clauses
        cin >> n >> m;
        int nodeCount = 2 * n;

        // 1. read the clauses and turn each one into two arrows
        vector<int> from(2 * m);
        vector<int> to(2 * m);
        for (int i = 0; i < m; i++) {
            int a, b;
            cin >> a >> b;                          // clause (a OR b)
            int nodeA = nodeOfLiteral(a);
            int nodeB = nodeOfLiteral(b);
            from[2 * i] = negationOf(nodeA);        // NOT a -> b
            to[2 * i] = nodeB;
            from[2 * i + 1] = negationOf(nodeB);    // NOT b -> a
            to[2 * i + 1] = nodeA;
        }

        // 2. build the graph and find its components
        vector<int> start, adj, comp;
        buildAdjacency(nodeCount, from, to, start, adj);
        findComponents(nodeCount, start, adj, comp);

        // 3. unsatisfiable if some x_i and NOT x_i share a component.
        //    (All input of this test case has been read, so we can go straight to the next one.)
        bool satisfiable = true;
        for (int i = 0; i < n; i++) {
            int notXi = 2 * i;
            int xi = 2 * i + 1;
            if (comp[notXi] == comp[xi]) {
                satisfiable = false;
                break;
            }
        }
        if (!satisfiable) {
            cout << "Unsatisfiable\n";
            continue;
        }

        // 4. x_i = 1 when x_i has the smaller component id than NOT x_i
        for (int i = 0; i < n; i++) {
            if (i > 0) {
                cout << ' ';
            }
            if (comp[2 * i + 1] < comp[2 * i]) {
                cout << 1;
            } else {
                cout << 0;
            }
        }
        cout << '\n';
    }
    return 0;
}
