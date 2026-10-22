// Q2 : Red-black tree with order statistics (k-th smallest key and rank of a key).
//
// A red-black tree is a binary search tree whose nodes are coloured red or black so that
//   1. the root is black
//   2. a red node never has a red child
//   3. every path from a node down to an empty spot contains the same number of black nodes
// These rules keep the tree height at most 2*log2(n+1), so insert and delete take O(log n).
//
// Order statistics: every node also stores 'size' = the number of nodes in its own subtree.
//
//          20 (size 3)          20 has 1 key on its left, so it is the 2nd smallest key.
//         /  \                  select(2) = 20
//    10 (1)  30 (1)             rank(25) = 2  (keys 10 and 20 are smaller than 25)
//
// Whenever the shape of the tree changes (insert, delete, rotation) the sizes are repaired,
// otherwise select and rank would give wrong answers.
//
// Example: insert 10, 20, 30 in this order.
//   10 becomes the root (black).  20 goes to the right of 10 (red).  30 goes to the right of 20 (red).
//   Now 20 and 30 are two reds in a row and the uncle (the empty left child of 10) is black,
//   so one left rotation at 10 plus a recolouring fixes it:  20 (black) with children 10 (red), 30 (red).
//   Sizes: 20 -> 3, 10 -> 1, 30 -> 1.   select(1) = 10, select(3) = 30, rank(25) = 2, rank(5) = 0.

#include <iostream>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int key;
    Color color;
    int size;                   // number of nodes in the subtree rooted here (the empty leaf NIL has size 0)
    Node *left;
    Node *right;
    Node *parent;
};

Node NIL_NODE;                  // one shared black "empty leaf" used instead of NULL pointers
Node *NIL = &NIL_NODE;
Node *root = NIL;               // an empty tree is a root that points to NIL

// Create a new node for 'key': red, size 1, and every pointer set to NIL.
Node *makeNode(int key) {
    Node *node = new Node;
    node->key = key;
    node->color = RED;          // new nodes start red (this never changes the black counts)
    node->size = 1;
    node->left = NIL;
    node->right = NIL;
    node->parent = NIL;
    return node;
}

// ---------------------------------------------------------------------------------------------
// Rotations: change the shape locally but keep the binary-search-tree order.
//
//          x                         y
//         / \      leftRotate       / \       <- children of the top node
//        a   y     ========>       x   c
//           / \                   / \       <- grandchildren
//          b   c                 a   b
//
// After a rotation only two sizes are wrong, and both are easy to fix:
//   y now covers exactly the nodes x used to cover, so  y.size = old x.size
//   x lost y's right part, so                           x.size = left.size + right.size + 1
// ---------------------------------------------------------------------------------------------
void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;                       // subtree b moves over to x
    if (y->left != NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;                    // y takes x's place under x's parent
    if (x->parent == NIL) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;

    y->size = x->size;                        // repair the two sizes
    x->size = x->left->size + x->right->size + 1;
}

// The mirror image of leftRotate.
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

    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}

// Ordinary binary-search-tree lookup; returns NIL when the key is absent.
Node *findNode(int key) {
    Node *current = root;
    while (current != NIL && current->key != key) {
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current;
}

// ---------------------------------------------------------------------------------------------
// (a) Insertion
// 1. Walk down like in a normal BST and attach the new RED node as a leaf.
//    Every node we pass on the way will contain the new key, so its size goes up by 1.
// 2. A red node under a red parent breaks rule 2. insertFixup repairs this:
//      case 1  uncle is red   : recolour parent, uncle and grandparent, then continue from the grandparent
//      case 2  uncle is black and the node is an "inner" child (zig-zag): rotate it into a straight line
//      case 3  uncle is black and the node is an "outer" child (straight line): recolour + one rotation
// Example: insert 30, 20, 10 in this order. After 10 we have the straight line 30 -> 20 -> 10 (case 3):
//          30(B)                 20(B)
//          /           ==>      /     \    <- 20 is the new top with two red children
//        20(R)               10(R)   30(R)
//        /
//      10(R)
// ---------------------------------------------------------------------------------------------
void insertFixup(Node *z) {
    // z is the red node that may sit under a red parent; keep repairing while that is the case
    while (z->parent->color == RED) {
        Node *grandparent = z->parent->parent;   // exists, because a red parent is never the root
        if (z->parent == grandparent->left) {
            Node *uncle = grandparent->right;
            if (uncle->color == RED) {
                // case 1: example 30(B) with red children 20 and 40, new red node under 20:
                // 20 and 40 become black, 30 becomes red, and we check 30 against ITS parent next
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    // case 2: z is a right child of a left child (a zig-zag), e.g. 30 -> 10 -> 20.
                    // A left rotation at 10 turns it into the straight line 30 -> 20 -> 10
                    z = z->parent;
                    leftRotate(z);
                }
                // case 3: straight line. The parent becomes black, the grandparent red, and one rotation
                // at the grandparent lifts the parent to the top
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            // the same three cases with left and right swapped
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
    root->color = BLACK;                      // rule 1: the root is always black
}

// The caller has already checked that the key is not in the tree.
void rbInsert(int key) {
    Node *newNodePtr = makeNode(key);
    Node *parent = NIL;
    Node *current = root;
    while (current != NIL) {
        current->size++;                      // the new key will land inside this subtree
        parent = current;
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    newNodePtr->parent = parent;
    if (parent == NIL) {
        root = newNodePtr;
    } else if (key < parent->key) {
        parent->left = newNodePtr;
    } else {
        parent->right = newNodePtr;
    }
    insertFixup(newNodePtr);
}

// ---------------------------------------------------------------------------------------------
// (b) Deletion
// Remove the node like in a normal BST (a node with two children is replaced by its successor,
// the smallest key of its right subtree). If the colour that disappeared was BLACK, one path has lost a
// black node and deleteFixup repairs the black counts with the four classic cases:
//      case 1  the sibling is red          : rotate so the sibling becomes black
//      case 2  sibling and both its children are black : recolour the sibling red, move the problem up
//      case 3  sibling black, its far child black, near child red : rotate the sibling
//      case 4  sibling black, its far child red : recolour + rotation, problem solved
// ---------------------------------------------------------------------------------------------

// Put the subtree v where the subtree u used to hang under its parent.
void transplant(Node *u, Node *v) {
    if (u->parent == NIL) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

// The smallest key of a subtree is its leftmost node.  Example: subtree 20 with children 10 and 30 -> 10.
Node *treeMinimum(Node *x) {
    while (x->left != NIL) {
        x = x->left;
    }
    return x;
}

// x is the node that took the place of a removed BLACK node, so every path through x is one black short:
// we say x carries an 'extra black'. The loop either pushes this extra black up the tree (case 2) or
// gets rid of it with a rotation (cases 1, 3, 4). When x becomes red or the root it is simply coloured black.
// Example: delete a black leaf whose sibling is black with two black children -> case 2:
// the sibling turns red (now both sides are equally short) and the problem moves up to the parent.
void deleteFixup(Node *x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node *sibling = x->parent->right;
            // sibling = the other child of x's parent; its colour decides which of the four cases applies
            if (sibling->color == RED) {
                // case 1: a red sibling is turned black by a rotation, so we can continue with a black sibling
                sibling->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                sibling = x->parent->right;
            }
            if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
                // case 2
                sibling->color = RED;
                x = x->parent;
            } else {
                if (sibling->right->color == BLACK) {
                    // case 3
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    rightRotate(sibling);
                    sibling = x->parent->right;
                }
                // case 4: the sibling's far child is red. After this recolouring and one rotation the
                // missing black is back and the loop ends (x = root)
                sibling->color = x->parent->color;
                x->parent->color = BLACK;
                sibling->right->color = BLACK;
                leftRotate(x->parent);
                x = root;
            }
        } else {
            // mirror image
            Node *sibling = x->parent->left;
            if (sibling->color == RED) {
                sibling->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                sibling = x->parent->left;
            }
            if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
                sibling->color = RED;
                x = x->parent;
            } else {
                if (sibling->left->color == BLACK) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    leftRotate(sibling);
                    sibling = x->parent->left;
                }
                sibling->color = x->parent->color;
                x->parent->color = BLACK;
                sibling->left->color = BLACK;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

// Remove node z from the tree and repair sizes and colours.
// Example: delete 20 from the tree 20 (black) with children 10 (red) and 30 (red). z has two children, so
// its successor 30 (the smallest key of the right subtree) takes its place and keeps z's colour. The
// removed colour was red, so no colour repair is needed; only the sizes on the path to the root change.
void rbDelete(Node *z) {
    Node *y = z;                              // the node that is really removed or moved
    Node *x;                                  // the node that takes y's old place
    Color yOriginalColor = y->color;

    if (z->left == NIL) {                     // no left child: the right child simply moves up
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == NIL) {             // no right child: the left child moves up
        x = z->left;
        transplant(z, z->left);
    } else {                                  // two children: the successor takes z's place
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    // Repair the sizes: x->parent is the lowest node whose subtree changed, so recompute
    // size = left.size + right.size + 1 on the way from there up to the root.
    // Example: deleting a leaf below node A below the root: A's size drops by 1, then the root's size.
    for (Node *walker = x->parent; walker != NIL; walker = walker->parent) {
        walker->size = walker->left->size + walker->right->size + 1;
    }
    if (yOriginalColor == BLACK) {
        deleteFixup(x);
    }
    // x may have been NIL and borrowed a parent pointer during the fix-up: reset the shared sentinel
    NIL->parent = NIL;
    NIL->size = 0;
    NIL->color = BLACK;
    delete z;
}

// ---------------------------------------------------------------------------------------------
// (c) k-th smallest key (k starts at 1)
// At a node, 'leftSize' keys are smaller than it:
//    k == leftSize + 1  -> this node is the answer
//    k <= leftSize      -> the answer is the k-th smallest of the left subtree
//    otherwise          -> skip the left subtree and this node: look for the (k - leftSize - 1)-th on the right
// Example: keys {10, 20, 30, 40, 50} with root 30 (leftSize = 2). select(4): 4 > 3, so go right with
//          k = 4 - 2 - 1 = 1 and take the 1st smallest of {40, 50} = 40.
// ---------------------------------------------------------------------------------------------
int select(int k) {
    Node *current = root;
    while (current != NIL) {
        int leftSize = current->left->size;
        if (k == leftSize + 1) {
            return current->key;
        }
        if (k <= leftSize) {
            current = current->left;
        } else {
            k = k - leftSize - 1;
            current = current->right;
        }
    }
    return 0;                                  // never reached when 1 <= k <= number of keys
}

// ---------------------------------------------------------------------------------------------
// (d) rank of x = number of stored keys strictly smaller than x (x does not have to be in the tree).
// Walk down as if searching for x. Every time we go RIGHT, the current node and its whole left subtree
// are smaller than x, so we add leftSize + 1.
// Example: keys {10, 20, 30, 40, 50}, rank(35): root 30 < 35 -> add 2 + 1 = 3 and go right;
//          40 >= 35 -> go left (empty). Answer 3 (the keys 10, 20, 30).
// ---------------------------------------------------------------------------------------------
int rankOf(int x) {
    int smaller = 0;
    Node *current = root;
    while (current != NIL) {
        if (x <= current->key) {
            current = current->left;
        } else {
            smaller = smaller + current->left->size + 1;
            current = current->right;
        }
    }
    return smaller;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    NIL->color = BLACK;
    NIL->size = 0;
    NIL->left = NIL;
    NIL->right = NIL;
    NIL->parent = NIL;

    // read the initial keys and insert them one by one
    int n;
    if (!(cin >> n)) {
        return 0;
    }
    for (int i = 0; i < n; i++) {
        int key;
        cin >> key;
        if (findNode(key) == NIL) {            // keys are distinct, but be safe
            rbInsert(key);
        }
    }

    // answer the queries: 1 = insert, 2 = delete, 3 = k-th smallest, 4 = rank
    int q;
    cin >> q;
    while (q--) {
        int type, x;
        cin >> type >> x;
        if (type == 1) {
            if (findNode(x) != NIL) {
                cout << x << " is already present. So no need to insert\n";
            } else {
                rbInsert(x);
                cout << x << " is inserted\n";
            }
        } else if (type == 2) {
            Node *target = findNode(x);
            if (target == NIL) {
                cout << x << " is not present. So it can not be deleted\n";
            } else {
                rbDelete(target);
                cout << x << " is deleted\n";
            }
        } else if (type == 3) {
            cout << select(x) << '\n';
        } else {
            cout << rankOf(x) << '\n';
        }
    }
    return 0;
}
