=================================================================
CS6013 - ADSA - Coding Assignment 1
=================================================================
Name    : Rohit Yadav
Roll No : CS26MTECH02007

=================================================================
FILES
=================================================================
Q1.cpp   - Binary Search Tree
Q2.cpp   - AVL Tree
Q3.cpp   - Red-Black Tree
Q4.cpp   - 2-4 Tree
Q5.cpp   - Perfect Hashing
Q6.cpp   - Max sum rank-consistent (BST-valid) subtree
Q7.cpp   - Nodes at distance K in an AVL tree
Q8.cpp   - Structural comparison of AVL and Red-Black trees
Q9.cpp   - K-th smallest key in a dynamic BST
Q10.cpp  - Count of alternative insertion histories of a BST
README.txt - this file

=================================================================
COMPILER
=================================================================
g++ (GNU GCC), C++17 standard
Version : g++.exe (MinGW.org GCC-6.3.0-1) 6.3.0
OS tested on : Windows

=================================================================
HOW TO COMPILE
=================================================================
    g++ -std=c++17 -O2 -o Q1 Q1.cpp
    g++ -std=c++17 -O2 -o Q2 Q2.cpp
    g++ -std=c++17 -O2 -o Q3 Q3.cpp
    g++ -std=c++17 -O2 -o Q4 Q4.cpp
    g++ -std=c++17 -O2 -o Q5 Q5.cpp
    g++ -std=c++17 -O2 -o Q6 Q6.cpp
    g++ -std=c++17 -O2 -o Q7 Q7.cpp
    g++ -std=c++17 -O2 -o Q8 Q8.cpp
    g++ -std=c++17 -O2 -o Q9 Q9.cpp
    g++ -std=c++17 -O2 -o Q10 Q10.cpp

=================================================================
HOW TO RUN
=================================================================
Q1, Q2, Q3, Q4, Q6, Q7, Q8, Q9, Q10:

  Run directly and type input at the terminal, e.g.:
      ./Q1
      (type each input line, press Enter after each)

Q5:
  
      ./Q5.exe T10I4D100K.dat > q5_output.txt

=================================================================
NOTES
=================================================================
Q1 - Binary Search Tree
  The optional parent pointer is used. It is set on every new
  child during insertion, and re-linked correctly in all three
  delete cases (leaf removal, single-child splice, and the
  two-child case where the inorder successor's value is copied up
  and the successor itself is removed from the right subtree).

Q2 - AVL Tree
  Both optional fields (height, parent) are used. Height is
  recomputed bottom-up via a refreshHeight() helper immediately
  after insert/delete, before the balance factor is checked.
  Rotations fix parent pointers internally; main() additionally
  resets the root's parent to nullptr after every top-level
  operation, since recursion alone does not guarantee this for
  whichever node ends up as the new root.

Q3 - Red-Black Tree
  The optional parent field is used, maintained via a transplant()
  helper plus the standard CLRS rotation and fix-up logic. The
  optional black_depth field is not included in the struct, since
  the CLRS algorithm only needs the color field to operate
  correctly. NIL leaves are represented as nullptr rather than a
  sentinel node, with small helper functions treating null as
  black wherever the algorithm checks a node's color.

Q4 - 2-4 Tree
  The optional parent pointer is used and updated after every
  split, borrow, and merge. Insertion is top-down: any full
  (3-key) node encountered on the way down is split immediately,
  including the root. Deletion is also top-down and follows the
  exact priority order specified in the assignment before
  descending into a 1-key child: borrow from the left sibling,
  then the right sibling, then merge with the right sibling, then
  merge with the left sibling.

Q5 - Perfect Hashing
  The dataset file itself is not part of this submission. The
  primary, required mode reads N, the flattened stream, and the
  random set entirely from standard input, as specified; a second
  convenience mode (raw dataset file as a command-line argument)
  is included only for easier local testing and does not change
  the required stdin behavior. Hash family used:
  h(x) = ((a*x+b) mod p) mod m, with a separate second-level table
  of size (bucket size)^2 per non-empty bucket, re-randomized until
  collision-free. The code uses .first/.second instead of C++17
  structured bindings, since the compiler here (GCC 6.3.0) predates
  GCC 7, which structured bindings require regardless of the -std
  flag used. Verified end-to-end against the real T10I4D100K
  dataset in both modes: RESULT: CORRECT, zero false positives and
  zero false negatives.

Q6 - Maximum Rank-Consistent Reporting Unit
  A single bottom-up DFS solves this in O(n): each recursive call
  returns {isBST, min, max, sum} for its subtree, so the parent
  can check the BST-validity condition against its own value in
  O(1) rather than re-scanning the subtree. Sums are accumulated
  in long long, since node count and value magnitude can together
  exceed the 32-bit range.

Q7 - Nodes at Distance K in an AVL Tree
  The AVL tree is built through genuine repeated insertion with
  rotations, not by sorting the keys and constructing a balanced
  tree directly, as the assignment requires. Finding the
  distance-K nodes is done with a single DFS that returns the
  distance from the current node down to the target (or -1 if not
  found on that side), collecting qualifying nodes along the way -
  O(n) overall, on top of O(log n) per insertion.

Q8 - Structural Comparison of AVL and Red-Black Trees
  AVL rebalancing follows the assignment's rule of using a single
  rotation when the heavy child's balance factor is zero. The
  Red-Black tree in this file uses a shared NIL sentinel node
  rather than nullptr (a different style from Q3, purely an
  implementation choice for this comparison), but follows the same
  standard CLRS algorithm. The comparison itself walks both trees
  together by structural position (the same left/right path from
  the root), ignoring color, and keeps recursing into both
  subtrees even where a mismatch is found, as required.

Q9 - K-th Smallest Key (Order Statistics BST)
  Each node's subtreeSize is maintained incrementally on every
  insert and delete via a small refreshSize() helper. The k-th
  smallest query then uses the standard subtree-size-guided
  selection algorithm, which takes O(h) and never performs a full
  inorder traversal or copies keys into an array.

Q10 - Alternative Insertion Histories of a BST
  Big integers are implemented manually as a vector<long long> in
  base 10^9, since the result can far exceed the 64-bit range for
  n up to 1000. The count itself follows the standard
  product-of-binomial-coefficients recurrence over the shape of
  the BST built from the input permutation, with both the
  binomial coefficients and the per-node results memoized; the
  final answer subtracts 1 to exclude the original input
  permutation from the count.

=================================================================
END OF README
=================================================================