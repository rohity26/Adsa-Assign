// =====================================================================================================
// Q4 : 2-CNF satisfiability  (is there an assignment of 0/1 to x_1..x_n making every clause true?)
// =====================================================================================================
//
// IDEA IN FOUR STEPS
//   1. A clause (a OR b) says: "if a is false then b must be true" and "if b is false then a must be true".
//      So every clause becomes TWO implication arrows:      NOT a -> b      and      NOT b -> a.
//      The graph has 2n nodes (every variable x_i gives two nodes: x_i and NOT x_i) and 2m arrows.
//   2. An arrow p -> q means "if p is true then q is forced to be true".
//      If p can reach q AND q can reach p (they lie on a common loop of arrows) then p and q are forced to
//      have the SAME truth value.  Such a maximal group of nodes is a strongly connected component (SCC).
//      We find all SCCs with Tarjan's algorithm (written iteratively, see below).
//   3. If x_i and NOT x_i are in the SAME SCC then x_i must equal NOT x_i, which is impossible
//      -> the formula is UNSATISFIABLE.   (We stop checking and print "Unsatisfiable".)
//   4. Otherwise a solution always exists and we build it:  for every variable compare the SCC ids of its two
//      literals and make TRUE the literal whose id is SMALLER  (that literal is "further downstream").
//
// WHY STEP 4 IS SAFE (short proof)
//   Tarjan gives ids in REVERSE topological order: if there is an arrow from component A to a different
//   component B, then id(B) < id(A).  Let rank = -id, so arrows never decrease rank.
//   We make true the literal with the larger rank.  Suppose a true literal l had an arrow l -> m with m false,
//   i.e. NOT m is the chosen (true) literal.  The mirror arrow NOT m -> NOT l also exists, and we get
//       rank(NOT l) < rank(l) <= rank(m) < rank(NOT m) <= rank(NOT l)      which is impossible.
//   So a true literal never points to a false one; hence no clause (a OR b) can have both literals false
//   (that would need the arrow NOT a -> b from a true literal to a false one).
//
// NODE NUMBERING (0-based variable index i = (variable number) - 1)
//       node 2*i + 1   means   x_i          node 2*i   means   NOT x_i
//   Negating a literal is therefore just  node ^ 1  (flip the lowest bit: 2i <-> 2i+1).
//
// WORKED EXAMPLE = first test of the assignment:  n = 3,  clauses  (x1 OR -x2), (-x1 OR -x3), (-x1 OR x2)
//   node numbers :  0:-x1   1:x1   2:-x2   3:x2   4:-x3   5:x3
//   arrows       :  -x1 -> -x2     x2 -> x1        (from clause 1)
//                   x1  -> -x3     x3 -> -x1       (from clause 2)
//                   x1  -> x2      -x2 -> -x1      (from clause 3)
//   SCC ids      :  {-x1,-x2} = 0    {x3} = 1    {-x3} = 2    {x1,x2} = 3
//   answer       :  x1: id(x1)=3, id(-x1)=0 -> 3 is not smaller -> 0
//                   x2: id(x2)=3, id(-x2)=0 -> 0
//                   x3: id(x3)=1, id(-x3)=2 -> 1 is smaller     -> 1          output "0 0 1"
//   Second test (all four sign combinations of x1,x2): all four literals end up in ONE component, so
//   x1 and -x1 share a component -> "Unsatisfiable".
//
// COMPLEXITY : O(n + m) per test case (building the graph, one Tarjan pass, one pass over the variables).
//   The sums of n and of m over all test cases are at most 2*10^5, so allocating the vectors inside the
//   test-case loop is fine.  Repeated literals / repeated clauses only create duplicate arrows (harmless).
// =====================================================================================================
#include <iostream>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);        // fast input/output
    int t; if (!(cin >> t)) return 0;                  // number of test cases
    while (t--) {
        int n, m; cin >> n >> m;                       // n variables, m clauses
        int V = 2 * n;                                 // number of graph nodes (a node for every literal)

        // ---------------------------------------------------------------------------------------------
        // STEP 1: read the clauses and store the 2m implication arrows as an edge list (from[e] -> to[e]).
        //   Input literal k > 0 means x_k      -> node 2*(k-1)+1
        //   Input literal k < 0 means NOT x_|k| -> node 2*(-k-1)
        //   e.g. "1" -> node 1,  "-1" -> node 0,  "2" -> node 3,  "-2" -> node 2,  "3" -> 5,  "-3" -> 4
        // ---------------------------------------------------------------------------------------------
        vector<int> from(2 * m), to(2 * m);
        for (int i = 0; i < m; i++) {
            int a, b; cin >> a >> b;                                   // clause (a OR b)
            int la = (a > 0) ? 2 * (a - 1) + 1 : 2 * (-a - 1);         // node of literal a
            int lb = (b > 0) ? 2 * (b - 1) + 1 : 2 * (-b - 1);         // node of literal b
            from[2 * i] = la ^ 1;     to[2 * i] = lb;                  // NOT a -> b   (if a is false, b must be true)
            from[2 * i + 1] = lb ^ 1; to[2 * i + 1] = la;              // NOT b -> a   (if b is false, a must be true)
        }

        // ---------------------------------------------------------------------------------------------
        // STEP 2: turn the edge list into a compact adjacency array (CSR format) using counting sort.
        //   After this, the nodes reachable from v by one arrow are   adj[ start[v] ... start[v+1]-1 ].
        // ---------------------------------------------------------------------------------------------
        vector<int> start(V + 1, 0), adj(2 * m);
        for (int e = 0; e < 2 * m; e++) start[from[e] + 1]++;          // count the out-degree of every node
        for (int v = 0; v < V; v++) start[v + 1] += start[v];          // prefix sums: start[v] = first slot of v
        vector<int> fillPos(start.begin(), start.end() - 1);           // next free slot inside each node's block
        for (int e = 0; e < 2 * m; e++) adj[fillPos[from[e]]++] = to[e];   // drop every arrow into its block

        // ---------------------------------------------------------------------------------------------
        // STEP 3: Tarjan's SCC algorithm, written with explicit stacks (no recursion), so that a chain of
        //         2*10^5 implications cannot overflow the call stack.
        //
        //   index[v]   : the order in which v was first visited (0,1,2,...); -1 means "not visited yet".
        //   low[v]     : the smallest index[] that v's search can reach by going down tree edges and then
        //                using at most one edge back into a node that is still waiting on sccStack.
        //                v is the ROOT of its component exactly when low[v] == index[v].
        //   comp[v]    : the id of the component v belongs to (-1 until it is assigned).
        //                Ids are handed out in the order components FINISH: 0, 1, 2, ...
        //                A component finishes only after every component it points to has finished, so an
        //                arrow from component A to a different component B always has id(B) < id(A).
        //   edgePos[v] : the next arrow of v to examine (this replaces the loop counter that a recursive
        //                version keeps on the call stack).
        //   callStack  : the nodes of the DFS path currently being explored (simulates the recursion).
        //   sccStack   : nodes visited but not yet assigned to a component.
        //   onStack[v] : 1 while v is on sccStack (used to tell "same component in progress" from "finished").
        // ---------------------------------------------------------------------------------------------
        vector<int> index(V, -1), low(V, 0), comp(V, -1), edgePos(V, 0), sccStack, callStack;
        vector<char> onStack(V, 0);
        int counter = 0, compCount = 0;                // counter = next visit number, compCount = next component id

        for (int s = V - 1; s >= 0; s--) {             // start a DFS from every unvisited node
                                                       // (decreasing order only affects WHICH valid answer is
                                                       //  printed; it makes our output equal the sample output)
            if (index[s] != -1) continue;              // already visited in an earlier DFS

            // "call" DFS(s): mark s visited and put it on both stacks
            callStack.push_back(s);
            index[s] = low[s] = counter++; edgePos[s] = start[s];
            sccStack.push_back(s); onStack[s] = 1;

            while (!callStack.empty()) {
                int v = callStack.back();              // the node we are currently working on

                if (edgePos[v] < start[v + 1]) {       // ---- v still has an unexamined outgoing arrow v -> w
                    int w = adj[edgePos[v]++];         // take it and advance v's pointer
                    if (index[w] == -1) {              // w never visited: this is a tree edge, "recurse" into w
                        index[w] = low[w] = counter++; edgePos[w] = start[w];
                        sccStack.push_back(w); onStack[w] = 1; callStack.push_back(w);
                    }
                    // w was visited before.  If it is still on sccStack it is unassigned, i.e. in the same
                    // component as v's current search, so v can reach back to w's index: lower low[v].
                    // (If w is not on sccStack its component is already finished, so it cannot belong to
                    //  v's component: ignore this arrow.)
                    else if (onStack[w] && index[w] < low[v]) low[v] = index[w];
                } else {                               // ---- all arrows of v are done: v is finished ("return")
                    callStack.pop_back();
                    if (!callStack.empty()) {          // report v's low value to its parent p in the DFS tree
                        int p = callStack.back();
                        if (low[v] < low[p]) low[p] = low[v];
                    }
                    if (low[v] == index[v]) {          // v is the root of a component: nothing below v reaches
                        int w;                         // anything earlier, so everything above v on sccStack
                        do {                           // (down to v) is exactly one SCC
                            w = sccStack.back(); sccStack.pop_back();
                            onStack[w] = 0;
                            comp[w] = compCount;       // give every member the same id
                        } while (w != v);
                        compCount++;                   // the next component to finish gets the next id
                    }
                }
            }
        }

        // ---------------------------------------------------------------------------------------------
        // STEP 4: unsatisfiable test.  x_i is node 2i+1 and NOT x_i is node 2i; if they share a component
        //         then x_i is forced to equal NOT x_i, which is impossible.
        //         All m clauses of this test case were already read above, so it is safe to 'continue'
        //         straight to the next test case.
        // ---------------------------------------------------------------------------------------------
        bool satisfiable = true;
        for (int i = 0; i < n; i++) if (comp[2 * i] == comp[2 * i + 1]) { satisfiable = false; break; }
        if (!satisfiable) { cout << "Unsatisfiable\n"; continue; }

        // ---------------------------------------------------------------------------------------------
        // STEP 5: build the assignment.  For variable i print 1 (true) when x_i has the SMALLER component id
        //         than NOT x_i, otherwise 0.  Smaller id = further downstream = forces fewer other literals,
        //         which is why this choice never contradicts an arrow (proof in the header comment).
        // ---------------------------------------------------------------------------------------------
        for (int i = 0; i < n; i++) { if (i) cout << ' '; cout << (comp[2 * i + 1] < comp[2 * i] ? 1 : 0); }
        cout << '\n';
    }
    return 0;
}
