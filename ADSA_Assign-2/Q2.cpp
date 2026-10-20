// Q2: Red-black tree with order statistics (CLRS 13 + 14.1).
// Every node stores the size of its subtree, so k-th smallest and rank take O(log n).
#include <iostream>
using namespace std;

enum Color { RED, BLACK };
struct Node {
    int key;
    Color color;
    int size;                 // number of nodes in this subtree (NIL has size 0)
    Node *left, *right, *parent;
};

Node NIL_NODE;                // shared black sentinel leaf
Node *NIL = &NIL_NODE;
Node *root = NIL;

Node *makeNode(int key) {
    Node *z = new Node;
    z->key = key; z->color = RED; z->size = 1;
    z->left = z->right = z->parent = NIL;
    return z;
}
// Rotations also repair the two size fields that change.
void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x; x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}
void rightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NIL) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) root = y;
    else if (x == x->parent->right) x->parent->right = y;
    else x->parent->left = y;
    y->right = x; x->parent = y;
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;
}
Node *findNode(int key) {
    Node *cur = root;
    while (cur != NIL && cur->key != key) cur = (key < cur->key) ? cur->left : cur->right;
    return cur;
}

// ---------- (a) insertion ----------
void insertFixup(Node *z) {
    while (z->parent->color == RED) {
        Node *gp = z->parent->parent;
        if (z->parent == gp->left) {
            Node *uncle = gp->right;
            if (uncle->color == RED) {                        // case 1: recolor and move up
                z->parent->color = BLACK; uncle->color = BLACK; gp->color = RED; z = gp;
            } else {
                if (z == z->parent->right) { z = z->parent; leftRotate(z); }   // case 2 -> case 3
                z->parent->color = BLACK; z->parent->parent->color = RED;       // case 3
                rightRotate(z->parent->parent);
            }
        } else {                                              // mirror image
            Node *uncle = gp->left;
            if (uncle->color == RED) {
                z->parent->color = BLACK; uncle->color = BLACK; gp->color = RED; z = gp;
            } else {
                if (z == z->parent->left) { z = z->parent; rightRotate(z); }
                z->parent->color = BLACK; z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}
void rbInsert(int key) {                     // caller guarantees key is absent
    Node *z = makeNode(key), *parent = NIL, *cur = root;
    while (cur != NIL) {
        cur->size++;                         // the new key lands in this subtree
        parent = cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    z->parent = parent;
    if (parent == NIL) root = z;
    else if (key < parent->key) parent->left = z;
    else parent->right = z;
    insertFixup(z);
}

// ---------- (b) deletion ----------
void transplant(Node *u, Node *v) {
    if (u->parent == NIL) root = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    v->parent = u->parent;
}
Node *treeMinimum(Node *x) { while (x->left != NIL) x = x->left; return x; }
void deleteFixup(Node *x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            Node *w = x->parent->right;
            if (w->color == RED) { w->color = BLACK; x->parent->color = RED; leftRotate(x->parent); w = x->parent->right; }
            if (w->left->color == BLACK && w->right->color == BLACK) { w->color = RED; x = x->parent; }
            else {
                if (w->right->color == BLACK) { w->left->color = BLACK; w->color = RED; rightRotate(w); w = x->parent->right; }
                w->color = x->parent->color; x->parent->color = BLACK; w->right->color = BLACK;
                leftRotate(x->parent); x = root;
            }
        } else {
            Node *w = x->parent->left;
            if (w->color == RED) { w->color = BLACK; x->parent->color = RED; rightRotate(x->parent); w = x->parent->left; }
            if (w->right->color == BLACK && w->left->color == BLACK) { w->color = RED; x = x->parent; }
            else {
                if (w->left->color == BLACK) { w->right->color = BLACK; w->color = RED; leftRotate(w); w = x->parent->left; }
                w->color = x->parent->color; x->parent->color = BLACK; w->left->color = BLACK;
                rightRotate(x->parent); x = root;
            }
        }
    }
    x->color = BLACK;
}
void rbDelete(Node *z) {
    Node *y = z, *x;
    Color yOriginalColor = y->color;
    if (z->left == NIL) { x = z->right; transplant(z, z->right); }
    else if (z->right == NIL) { x = z->left; transplant(z, z->left); }
    else {
        y = treeMinimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) x->parent = y;
        else { transplant(y, y->right); y->right = z->right; y->right->parent = y; }
        transplant(z, y);
        y->left = z->left; y->left->parent = y;
        y->color = z->color;
    }
    // x->parent is the lowest node whose subtree changed: recompute sizes from there to the root
    for (Node *w = x->parent; w != NIL; w = w->parent) w->size = w->left->size + w->right->size + 1;
    if (yOriginalColor == BLACK) deleteFixup(x);
    NIL->parent = NIL; NIL->size = 0; NIL->color = BLACK;   // keep the sentinel clean
    delete z;
}

// ---------- (c) k-th smallest, 1-based ----------
int select(int k) {
    Node *cur = root;
    while (cur != NIL) {
        int leftSize = cur->left->size;
        if (k == leftSize + 1) return cur->key;
        if (k <= leftSize) cur = cur->left;
        else { k -= leftSize + 1; cur = cur->right; }
    }
    return 0;   // unreachable when 1 <= k <= size
}
// ---------- (d) rank = number of keys strictly smaller than x (x may be absent) ----------
int rankOf(int x) {
    int smaller = 0;
    Node *cur = root;
    while (cur != NIL) {
        if (x <= cur->key) cur = cur->left;
        else { smaller += cur->left->size + 1; cur = cur->right; }
    }
    return smaller;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    NIL->color = BLACK; NIL->size = 0; NIL->left = NIL->right = NIL->parent = NIL;
    int n; if (!(cin >> n)) return 0;
    for (int i = 0; i < n; i++) { int k; cin >> k; if (findNode(k) == NIL) rbInsert(k); }
    int q; cin >> q;
    while (q--) {
        int type, x; cin >> type >> x;
        if (type == 1) {
            if (findNode(x) != NIL) cout << x << " is already present. So no need to insert\n";
            else { rbInsert(x); cout << x << " is inserted\n"; }
        } else if (type == 2) {
            Node *z = findNode(x);
            if (z == NIL) cout << x << " is not present. So it can not be deleted\n";
            else { rbDelete(z); cout << x << " is deleted\n"; }
        } else if (type == 3) cout << select(x) << '\n';
        else cout << rankOf(x) << '\n';
    }
    return 0;
}
