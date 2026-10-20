CS6013 - Advanced Data Structures and Algorithms
Coding Assignment 2
=====================================================================
Name     : Rohit Yadav
Roll No. : CS26MTECH02007

---------------------------------------------------------------------
COMPILER
---------------------------------------------------------------------
g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
Language standard: C++11 (-std=c++11). Only standard-library headers are used.

---------------------------------------------------------------------
FILES
---------------------------------------------------------------------
Q1.cpp  - B-tree: search, top-down insert, top-down delete, level-order display
Q2.cpp  - Red-black tree with order statistics (k-th smallest, rank)
Q3.cpp  - Red-black tree range enumeration RB-ENUMERATE(x, a, b)
Q3_report.txt - proof of correctness and running-time analysis for Question 3
Q4.cpp  - 2-CNF satisfiability (implication graph + strongly connected components)
Q5.cpp  - Forbidden shortcut (Dijkstra with exactly one negative road)
Q6.cpp  - Powering the cities (MST with a virtual source node, Kruskal)
Q7.cpp  - Mandatory roads in a minimum spanning tree (Kruskal + union-find)
Q8.cpp  - Circular pattern matching with KMP
Q9.cpp  - Polynomial multiplication with FFT   (the written answer for Q9(b) is at the end of this file)
README.txt - this file

---------------------------------------------------------------------
HOW TO COMPILE
---------------------------------------------------------------------
    g++ -std=c++11 -O2 -o Q1 Q1.cpp
    g++ -std=c++11 -O2 -o Q2 Q2.cpp
    g++ -std=c++11 -O2 -o Q3 Q3.cpp
    g++ -std=c++11 -O2 -o Q4 Q4.cpp
    g++ -std=c++11 -O2 -o Q5 Q5.cpp
    g++ -std=c++11 -O2 -o Q6 Q6.cpp
    g++ -std=c++11 -O2 -o Q7 Q7.cpp
    g++ -std=c++11 -O2 -o Q8 Q8.cpp
    g++ -std=c++11 -O2 -o Q9 Q9.cpp
(On Windows use "-o Qn.exe".)

---------------------------------------------------------------------
HOW TO RUN
---------------------------------------------------------------------
Every program reads standard input and writes standard output; none prints prompts.
Run a program and type (or redirect) its input in the format given in the assignment, e.g.

    ./Q1 < input.txt                      (Linux / macOS / Windows cmd.exe)
    cmd /c "Q1.exe < input.txt"           (Windows PowerShell)

The same pattern applies to Q2 ... Q9. The source files contain no hard-coded sample answers.

---------------------------------------------------------------------
NOTES ON EACH PROGRAM
---------------------------------------------------------------------
Q1  B-tree. Node layout follows the required struct (key array of MAX entries, count, children array of
    MAX+1 entries, unused child pointers are NULL). All the conventions of the statement are followed:
    top-down insertion (split a full root first, split a full child before descending, middle key goes up);
    top-down deletion (before descending into a child with MIN keys: borrow from the left sibling, else from
    the right sibling, else merge with the right sibling if it exists, else with the left sibling);
    internal-key deletion uses the predecessor if the left child has more than MIN keys, else the successor
    if the right child does, else merges both children with the separator; an empty internal root is replaced
    by its only child; an empty tree is a NULL root. Duplicate insertions and deletions of absent keys are
    detected by a search first, so they leave the tree completely unchanged.
    Each operation visits O(log_MIN n) nodes (the tree height) and does O(MAX) work per node.

Q2  Red-black tree whose every node also stores size = number of nodes in its subtree. Rotations repair the
    two affected sizes; insertion increments sizes along the search path; deletion recomputes sizes on the
    path from the lowest changed node to the root before the fix-up. k-th smallest descends using left-subtree
    sizes; rank(x) counts keys strictly smaller than x (x need not be present). All operations O(log n).

Q3  Same red-black tree (no augmentation). RB-ENUMERATE skips the left subtree when a >= x.key and the right
    subtree when x.key >= b, giving O(r + log n) per query. Proof and analysis: see Q3_report.txt (Question 3).

Q4  2-SAT. Clause (a OR b) gives implications (NOT a -> b) and (NOT b -> a). Strongly connected components are
    found with an iterative Tarjan algorithm (no recursion, so long implication chains cannot overflow the stack).
    Unsatisfiable iff some variable and its negation share a component; otherwise x_i = true iff the component
    of x_i comes later in topological order than that of NOT x_i. Time O(n + m) per test case.

Q5  Dijkstra is never run on a graph containing the negative road. The negative road (u0 -> v0, weight w0) is
    removed; Dijkstra runs from S and from v0 on the remaining non-negative graph. Answer =
    min( dist_S[T], dist_S[u0] + w0 + dist_v0[T] ). If neither is finite, UNREACHABLE. 64-bit arithmetic.
    Time O((N + M) log N).

Q6  Add a virtual node 0 joined to every city i by an edge of cost P_i (building a station = using that edge).
    Every city is electrified exactly when all cities are connected to node 0, so the answer is the weight of the
    MST of this (N+1)-node graph, found with Kruskal's algorithm and union-find. Time O((N + M) log (N + M)).

Q7  The mandatory roads are united first; if one of them closes a cycle the answer is -1. Kruskal then adds the
    remaining roads in increasing cost; if fewer than N-1 roads end up in the tree the answer is -1.
    Union-find with path compression and union by size, so O(M log(M+1) + (N+M) alpha(N)).

Q8  Circular matching: a circular window starting at i (0 <= i < N) is an ordinary window of the linear string
    S + S[0..M-2]. KMP (with the LPS / prefix-function array) is run on that string, so overlapping and wrapping
    matches are all found, each start index < N exactly once. Time O(N + M).

Q9  (a) Iterative radix-2 FFT. Both coefficient arrays are padded to L = smallest power of two >= n+m-1, transformed,
    multiplied point-wise, and inverse-transformed; each result is rounded to the nearest integer and printed as a
    64-bit value. Roots of unity are computed directly with cos/sin (not by repeated multiplication) for accuracy.
    Since |a_i|, |b_j| <= 100 and n, m <= 2*10^5, every |c_k| <= 2*10^9, which double precision represents exactly
    after rounding. Time O(L log L).

=====================================================================
Q9(b)  Kronecker product and its connection to FFT
=====================================================================
(i) Is the claim possible?  NO.
    The Kronecker product z = u (x) v of two n-dimensional vectors has n*n = n^2 entries, z_{in+j} = u_i v_j.
    Under the stated assumption that outputting one scalar takes constant time, merely writing z down takes
    Theta(n^2) time. Since n^2 is not O(n log n), no algorithm - FFT or otherwise - can explicitly output u (x) v
    in O(n log n) time. This is an output-size lower bound, so it holds regardless of how clever the algorithm
    is. (FFT cannot help because there is nothing to speed up: every one of the n^2 entries is a single
    product u_i * v_j, with no sums that could be shared or batched.)

(ii) Implicit representation in O(n) space with O(1) queries.
    Do not build z. Store only the two input vectors u[0..n-1] and v[0..n-1] (O(n) space). To query the entry
    at position p (0 <= p < n^2):  i = p / n (integer division),  j = p mod n,  and return u[i] * v[j].
    This is one division, one modulo, two array reads and one multiplication: O(1) time.
    (Equivalently, z is the matrix u v^T, a rank-one outer product, written out row by row; the implicit form
    stores the two factors instead of the n^2 products.)

(iii) Kronecker product versus polynomial convolution, and the role of FFT.
    Both start from the same n^2 products u_i v_j, but they treat them differently:
      * Kronecker product: every pair (i, j) gets its OWN output slot, in + j. Nothing is added. The output has
        n^2 entries, so it is fundamentally output-bound (see part (i)).
      * Convolution s_k = sum over i+j = k of u_i v_j: all products with the same index sum i+j are ADDED into one
        slot. The output has only 2n-1 entries - the coefficients of the product polynomial U(x)V(x). Viewing
        the n x n table of products u_i v_j, s_k is the sum of the k-th anti-diagonal of that table.
    FFT is useful for convolution precisely because convolution collapses n^2 products into 2n-1 sums: by the
    convolution theorem, evaluate U and V at the L-th roots of unity (FFT), multiply the two value-lists point-wise
    (L multiplications), and interpolate back (inverse FFT). That gives all s_k in O(n log n) without ever forming
    the n^2 products. The Kronecker product has no such summation structure to exploit, so FFT does not apply to
    computing it explicitly; the only way to get O(n log n)-or-better behaviour for it is the implicit
    representation of part (ii).
