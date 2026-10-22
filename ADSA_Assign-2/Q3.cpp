// Q3 : Range enumeration in a red-black tree.
//
// Task: print every key k of the tree with a <= k <= b, in increasing order,
// in O(r + log n) time, where r = number of keys printed and n = number of keys stored.
//
// Idea: an in-order walk (left subtree, node, right subtree) prints keys in sorted order.
// We only skip the parts of the tree that cannot contain any key of the range:
//   * all keys in the LEFT subtree of x are smaller than x.key, so if a >= x.key none of them is >= a
//     -> skip the left subtree
//   * all keys in the RIGHT subtree of x are bigger than x.key, so if x.key >= b none of them is <= b
//     -> skip the right subtree
//
// Example: keys 4 5 8 9 21 23 31 32 34 45 77 78 88 89 90 99 112, query [34, 88].
//   At the root 31: 31 < a=34, so the whole left part (all keys <= 31) is skipped, we go right.
//   At 77: 77 is inside the range -> its left side may hold 34..76, its right side may hold 78..88.
//   Only the nodes on the two paths leading to 34 and to 88, plus the printed keys, are ever visited.
//
// The red-black tree below is only used to guarantee a small height (<= 2*log2(n+1)); for this question
// only insertion is needed. A complete proof and the running-time analysis are in Q3_report.txt.

#include <iostream>
#include <vector>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int key;
    Color color;
    Node *left;
    Node *right;
    Node *parent;
};

Node NIL_NODE;                  // shared black empty leaf
Node *NIL = &NIL_NODE;
Node *root = NIL;

// Rotations keep the binary-search-tree order and are needed by the insertion fix-up.
//
//          x                         y
//         / \      leftRotate       / \       <- children of the top node
//        a   y     ========>       x   c
//           / \                   / \       <- grandchildren
//          b   c                 a   b
void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// The mirror image of leftRotate: the LEFT child y of x moves up and x becomes y's right child.
void rightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NIL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NIL) {
        root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// After adding a red leaf, two reds in a row may appear. Repair (3 cases, mirrored for the right side):
//   case 1: uncle is red                       -> recolour parent, uncle, grandparent; continue upwards
//   case 2: uncle black, node is a zig-zag     -> rotate it into a straight line
//   case 3: uncle black, node in straight line -> recolour and rotate the grandparent
// Example: insert 10, 20, 30. After 30 the red nodes 20 and 30 are in a row; the uncle (empty left child of 10)
// counts as black and 30 is a right child of a right child (a straight line), so we recolour (20 black,
// 10 red) and rotate left at 10: the result is 20 on top with the red children 10 and 30.
void insertFixup(Node *z) {
    // repeat while the new red node z has a red parent (a red parent is never the root, so a grandparent exists)
    while (z->parent->color == RED) {
        Node *grandparent = z->parent->parent;
        if (z->parent == grandparent->left) {
            // the parent is a LEFT child; its sibling is the 'uncle'
            Node *uncle = grandparent->right;
            if (uncle->color == RED) {
                // case 1: red uncle -> push the blackness down from the grandparent, then continue upwards
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    // case 2: z is a zig-zag, rotate so that it becomes a straight line (then case 3 applies)
                    z = z->parent;
                    leftRotate(z);
                }
                // case 3: straight line -> parent black, grandparent red, rotate the grandparent
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            // the parent is a RIGHT child: the same three cases with left and right swapped
            Node *uncle = grandparent->left;
            if (uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;                          // rule 1: the root is always black
}

// Insert a key as a red leaf (a key that is already stored is ignored), then repair the colours.
void rbInsert(int key) {
    // make a new red leaf
    Node *newNodePtr = new Node;
    newNodePtr->key = key;
    newNodePtr->color = RED;
    newNodePtr->left = NIL;
    newNodePtr->right = NIL;

    // walk down to the empty spot where the key belongs (like an ordinary binary search tree)
    Node *parent = NIL;
    Node *current = root;
    while (current != NIL) {
        parent = current;
        if (key == current->key) {
            delete newNodePtr;                // duplicate key: nothing to do
            return;
        }
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    // hang the new leaf below its parent (or make it the root of an empty tree)
    newNodePtr->parent = parent;
    if (parent == NIL) {
        root = newNodePtr;
    } else if (key < parent->key) {
        parent->left = newNodePtr;
    } else {
        parent->right = newNodePtr;
    }
    // a red leaf can break the 'no red node below a red node' rule: repair it
    insertFixup(newNodePtr);
}

// RB-ENUMERATE(x, a, b): collect the keys k of the subtree of x with a <= k <= b in increasing order.
//
// Worked example. Tree:      30              Query [32, 45]
//                           /  |
//                         20    40
//                              /  |
//                            35    50
//   at 30: a=32 < 30 is false  -> the left subtree (only 20) is skipped; 30 is not in range; 30 < 45 -> go right
//   at 40: a=32 < 40           -> go left to 35
//   at 35: a < 35 -> left side is empty; 35 is in range -> output 35; 35 < 45 -> right side is empty
//   back at 40: 40 is in range -> output 40; 40 < 45 -> go right to 50
//   at 50: left side is empty; 50 is not in range; 50 < 45 is false -> right side skipped
//   output: 35 40   (the node 20 was never looked at)
void rbEnumerate(Node *x, int a, int b, vector<int> &result) {
    if (x == NIL) {
        return;
    }
    if (a < x->key) {                         // the left subtree may contain keys >= a
        rbEnumerate(x->left, a, b, result);
    }
    if (a <= x->key && x->key <= b) {         // x itself is inside the range
        result.push_back(x->key);
    }
    if (x->key < b) {                         // the right subtree may contain keys <= b
        rbEnumerate(x->right, a, b, result);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    // the shared empty leaf is black and points to itself, so walking 'below' it never crashes
    NIL->color = BLACK;
    NIL->left = NIL;
    NIL->right = NIL;
    NIL->parent = NIL;

    // build the tree from the n keys, in the given insertion order
    int n;
    if (!(cin >> n)) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        int key;
        cin >> key;
        rbInsert(key);
    }

    // answer the range queries one by one; 'found' is reused so no memory is allocated per query
    int q;
    cin >> q;
    vector<int> found;
    while (q--) {
        int a, b;
        cin >> a >> b;
        found.clear();
        rbEnumerate(root, a, b, found);
        if (found.empty()) {                     // no key lies in [a, b]
            cout << "EMPTY\n";
            continue;
        }
        for (size_t i = 0; i < found.size(); i++) {
            if (i > 0) {
                cout << ' ';
            }
            cout << found[i];
        }
        cout << '\n';
    }
    return 0;
}
