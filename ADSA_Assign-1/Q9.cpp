#include <bits/stdc++.h>
using namespace std;

struct Node {
    int key;
    int subtreeSize;
    Node *left, *right;
};

Node* makeNode(int k) { return new Node{k, 1, nullptr, nullptr}; }

int subtreeSize(Node* n) { return n ? n->subtreeSize : 0; }
void refreshSize(Node* n) { if (n) n->subtreeSize = 1 + subtreeSize(n->left) + subtreeSize(n->right); }

Node* insertKey(Node* root, int key) {
    if (!root) return makeNode(key);
    if (key < root->key) root->left = insertKey(root->left, key);
    else if (key > root->key) root->right = insertKey(root->right, key);
    // equal key: duplicate, tree stays unchanged
    refreshSize(root);
    return root;
}

Node* findMinNode(Node* n) { while (n->left) n = n->left; return n; }

Node* deleteKey(Node* root, int key) {
    if (!root) return nullptr;
    if (key < root->key) root->left = deleteKey(root->left, key);
    else if (key > root->key) root->right = deleteKey(root->right, key);
    else {
        if (!root->left || !root->right) {
            Node* onlyChild = root->left ? root->left : root->right;
            delete root;
            return onlyChild;
        }
        Node* successor = findMinNode(root->right);
        root->key = successor->key;
        root->right = deleteKey(root->right, successor->key);
    }
    refreshSize(root);
    return root;
}

// Finds the k-th smallest key (1-indexed) using subtree sizes to
// decide which way to go, instead of walking the whole tree — O(h).
int kthSmallest(Node* root, int k) {
    if (!root) return INT_MIN; // shouldn't happen for a valid k
    int leftSize = subtreeSize(root->left);
    if (k == leftSize + 1) return root->key;
    if (k <= leftSize) return kthSmallest(root->left, k);
    return kthSmallest(root->right, k - leftSize - 1);
}

int main() {
    int q;
    cin >> q;
    Node* root = nullptr;

    while (q--) {
        char op;
        cin >> op;
        if (op == 'I') {
            int x;
            cin >> x;
            root = insertKey(root, x);
        } else if (op == 'D') {
            int x;
            cin >> x;
            root = deleteKey(root, x);
        } else { // 'K' — k-th smallest query
            int k;
            cin >> k;
            if (k > subtreeSize(root)) cout << "INVALID\n";
            else cout << kthSmallest(root, k) << "\n";
        }
    }

    return 0;
}
