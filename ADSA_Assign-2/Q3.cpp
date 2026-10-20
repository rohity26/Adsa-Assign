// Q3: Range enumeration RB-ENUMERATE(x, a, b) on a red-black tree.
// Visits only nodes that can lead to a key in [a, b]: O(r + log n) per query.
#include <iostream>
#include <vector>
using namespace std;

enum Color { RED, BLACK };
struct Node { int key; Color color; Node *left, *right, *parent; };
Node NIL_NODE; Node *NIL = &NIL_NODE; Node *root = NIL;

void leftRotate(Node *x) {
    Node *y = x->right; x->right = y->left;
    if (y->left != NIL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) root = y; else if (x == x->parent->left) x->parent->left = y; else x->parent->right = y;
    y->left = x; x->parent = y;
}
void rightRotate(Node *x) {
    Node *y = x->left; x->left = y->right;
    if (y->right != NIL) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL) root = y; else if (x == x->parent->right) x->parent->right = y; else x->parent->left = y;
    y->right = x; x->parent = y;
}
void insertFixup(Node *z) {
    while (z->parent->color == RED) {
        Node *gp = z->parent->parent;
        if (z->parent == gp->left) {
            Node *u = gp->right;
            if (u->color == RED) { z->parent->color = BLACK; u->color = BLACK; gp->color = RED; z = gp; }
            else { if (z == z->parent->right) { z = z->parent; leftRotate(z); }
                   z->parent->color = BLACK; z->parent->parent->color = RED; rightRotate(z->parent->parent); }
        } else {
            Node *u = gp->left;
            if (u->color == RED) { z->parent->color = BLACK; u->color = BLACK; gp->color = RED; z = gp; }
            else { if (z == z->parent->left) { z = z->parent; rightRotate(z); }
                   z->parent->color = BLACK; z->parent->parent->color = RED; leftRotate(z->parent->parent); }
        }
    }
    root->color = BLACK;
}
void rbInsert(int key) {
    Node *z = new Node; z->key = key; z->color = RED; z->left = z->right = NIL;
    Node *parent = NIL, *cur = root;
    while (cur != NIL) { parent = cur; if (key == cur->key) { delete z; return; } cur = (key < cur->key) ? cur->left : cur->right; }
    z->parent = parent;
    if (parent == NIL) root = z; else if (key < parent->key) parent->left = z; else parent->right = z;
    insertFixup(z);
}

// RB-ENUMERATE(x, a, b): in-order walk that skips subtrees which cannot contain keys in [a, b].
//  - the left subtree of x holds keys < x.key, so it can only help if a < x.key
//  - the right subtree holds keys > x.key, so it can only help if x.key < b
void rbEnumerate(Node *x, int a, int b, vector<int> &out) {
    if (x == NIL) return;
    if (a < x->key) rbEnumerate(x->left, a, b, out);
    if (a <= x->key && x->key <= b) out.push_back(x->key);
    if (x->key < b) rbEnumerate(x->right, a, b, out);
}

int main() {
    ios::sync_with_stdio(false); cin.tie(NULL);
    NIL->color = BLACK; NIL->left = NIL->right = NIL->parent = NIL;
    int n; if (!(cin >> n)) return 0;
    for (int i = 0; i < n; i++) { int k; cin >> k; rbInsert(k); }
    int q; cin >> q;
    vector<int> found;
    while (q--) {
        int a, b; cin >> a >> b;
        found.clear();
        rbEnumerate(root, a, b, found);
        if (found.empty()) { cout << "EMPTY\n"; continue; }
        for (size_t i = 0; i < found.size(); i++) { if (i) cout << ' '; cout << found[i]; }
        cout << '\n';
    }
    return 0;
}
