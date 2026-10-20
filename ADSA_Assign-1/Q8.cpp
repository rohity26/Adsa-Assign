#include <bits/stdc++.h>
using namespace std;

struct AVLNode
{
    int key, height;
    AVLNode *left, *right;
};
AVLNode *avlMakeNode(int k) { return new AVLNode{k, 1, nullptr, nullptr}; }
int avlHeight(AVLNode *n) { return n ? n->height : 0; }
void avlRefreshHeight(AVLNode *n)
{
    if (n)
        n->height = 1 + max(avlHeight(n->left), avlHeight(n->right));
}
int avlBalanceFactor(AVLNode *n) { return n ? avlHeight(n->left) - avlHeight(n->right) : 0; }
AVLNode *avlRotateLeft(AVLNode *x)
{
    AVLNode *y = x->right;
    x->right = y->left;
    y->left = x;
    avlRefreshHeight(x);
    avlRefreshHeight(y);
    return y;
}
AVLNode *avlRotateRight(AVLNode *y)
{
    AVLNode *x = y->left;
    y->left = x->right;
    x->right = y;
    avlRefreshHeight(y);
    avlRefreshHeight(x);
    return x;
}
AVLNode *avlRebalance(AVLNode *n)
{
    avlRefreshHeight(n);
    int balance = avlBalanceFactor(n);
    if (balance > 1)
    {
        if (avlBalanceFactor(n->left) < 0)
            n->left = avlRotateLeft(n->left);
        return avlRotateRight(n);
    }
    if (balance < -1)
    {
        if (avlBalanceFactor(n->right) > 0)
            n->right = avlRotateRight(n->right);
        return avlRotateLeft(n);
    }
    return n;
}
AVLNode *avlInsert(AVLNode *root, int key)
{
    if (!root)
        return avlMakeNode(key);
    if (key < root->key)
        root->left = avlInsert(root->left, key);
    else if (key > root->key)
        root->right = avlInsert(root->right, key);
    return avlRebalance(root);
}
AVLNode *avlFindMin(AVLNode *n)
{
    while (n->left)
        n = n->left;
    return n;
}
AVLNode *avlDelete(AVLNode *root, int key)
{
    if (!root)
        return nullptr;
    if (key < root->key)
        root->left = avlDelete(root->left, key);
    else if (key > root->key)
        root->right = avlDelete(root->right, key);
    else
    {
        if (!root->left || !root->right)
        {
            AVLNode *onlyChild = root->left ? root->left : root->right;
            delete root;
            return onlyChild;
        }
        // Use the inorder successor; if the heavier child has balance 0,
        // avlRebalance ends up doing a single rotation instead of a double one.
        AVLNode *successor = avlFindMin(root->right);
        root->key = successor->key;
        root->right = avlDelete(root->right, successor->key);
    }
    return avlRebalance(root);
}
bool avlSearch(AVLNode *root, int key)
{
    if (!root)
        return false;
    if (key == root->key)
        return true;
    return avlSearch(key < root->key ? root->left : root->right, key);
}

// ===== Red-Black Tree (CLRS style, using a shared NIL sentinel) =====
struct RBNode
{
    int key;
    bool isRed;
    RBNode *left, *right, *parent;
};

RBNode *NIL; // shared black sentinel standing in for every "null" leaf

void rbInitSentinel()
{
    NIL = new RBNode{0, false, nullptr, nullptr, nullptr};
    NIL->left = NIL->right = NIL->parent = NIL;
}

RBNode *rbMakeNode(int k)
{
    RBNode *n = new RBNode{k, true, NIL, NIL, NIL};
    return n;
}

bool isSentinel(RBNode *n) { return n == NIL; }

void rbRotateLeft(RBNode *&root, RBNode *x)
{
    RBNode *y = x->right;
    x->right = y->left;
    if (!isSentinel(y->left))
        y->left->parent = x;
    y->parent = x->parent;
    if (isSentinel(x->parent))
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}
void rbRotateRight(RBNode *&root, RBNode *y)
{
    RBNode *x = y->left;
    y->left = x->right;
    if (!isSentinel(x->right))
        x->right->parent = y;
    x->parent = y->parent;
    if (isSentinel(y->parent))
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}
void rbFixInsert(RBNode *&root, RBNode *z)
{
    while (z->parent->isRed)
    {
        if (z->parent == z->parent->parent->left)
        {
            RBNode *uncle = z->parent->parent->right;
            if (uncle->isRed)
            {
                z->parent->isRed = false;
                uncle->isRed = false;
                z->parent->parent->isRed = true;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rbRotateLeft(root, z);
                }
                z->parent->isRed = false;
                z->parent->parent->isRed = true;
                rbRotateRight(root, z->parent->parent);
            }
        }
        else
        {
            RBNode *uncle = z->parent->parent->left;
            if (uncle->isRed)
            {
                z->parent->isRed = false;
                uncle->isRed = false;
                z->parent->parent->isRed = true;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rbRotateRight(root, z);
                }
                z->parent->isRed = false;
                z->parent->parent->isRed = true;
                rbRotateLeft(root, z->parent->parent);
            }
        }
    }
    root->isRed = false;
}
void rbInsert(RBNode *&root, int key)
{
    RBNode *z = rbMakeNode(key);
    RBNode *parent = NIL, *cur = root;
    while (!isSentinel(cur))
    {
        parent = cur;
        if (z->key < cur->key)
            cur = cur->left;
        else if (z->key > cur->key)
            cur = cur->right;
        else
        {
            delete z;
            return;
        } // duplicate, ignore
    }
    z->parent = parent;
    if (isSentinel(parent))
        root = z;
    else if (z->key < parent->key)
        parent->left = z;
    else
        parent->right = z;
    rbFixInsert(root, z);
}
void rbTransplant(RBNode *&root, RBNode *target, RBNode *replacement)
{
    if (isSentinel(target->parent))
        root = replacement;
    else if (target == target->parent->left)
        target->parent->left = replacement;
    else
        target->parent->right = replacement;
    replacement->parent = target->parent;
}
RBNode *rbFindMin(RBNode *x)
{
    while (!isSentinel(x->left))
        x = x->left;
    return x;
}
void rbFixDelete(RBNode *&root, RBNode *x)
{
    while (x != root && !x->isRed)
    {
        if (x == x->parent->left)
        {
            RBNode *sibling = x->parent->right;
            if (sibling->isRed)
            {
                sibling->isRed = false;
                x->parent->isRed = true;
                rbRotateLeft(root, x->parent);
                sibling = x->parent->right;
            }
            if (!sibling->left->isRed && !sibling->right->isRed)
            {
                sibling->isRed = true;
                x = x->parent;
            }
            else
            {
                if (!sibling->right->isRed)
                {
                    sibling->left->isRed = false;
                    sibling->isRed = true;
                    rbRotateRight(root, sibling);
                    sibling = x->parent->right;
                }
                sibling->isRed = x->parent->isRed;
                x->parent->isRed = false;
                sibling->right->isRed = false;
                rbRotateLeft(root, x->parent);
                x = root;
            }
        }
        else
        {
            RBNode *sibling = x->parent->left;
            if (sibling->isRed)
            {
                sibling->isRed = false;
                x->parent->isRed = true;
                rbRotateRight(root, x->parent);
                sibling = x->parent->left;
            }
            if (!sibling->right->isRed && !sibling->left->isRed)
            {
                sibling->isRed = true;
                x = x->parent;
            }
            else
            {
                if (!sibling->left->isRed)
                {
                    sibling->right->isRed = false;
                    sibling->isRed = true;
                    rbRotateLeft(root, sibling);
                    sibling = x->parent->left;
                }
                sibling->isRed = x->parent->isRed;
                x->parent->isRed = false;
                sibling->left->isRed = false;
                rbRotateRight(root, x->parent);
                x = root;
            }
        }
    }
    x->isRed = false;
}
void rbDelete(RBNode *&root, int key)
{
    RBNode *target = root;
    while (!isSentinel(target) && target->key != key)
        target = key < target->key ? target->left : target->right;
    if (isSentinel(target))
        return; // not found

    RBNode *movedNode = target;
    bool origWasRed = movedNode->isRed;
    RBNode *fixupNode;
    if (isSentinel(target->left))
    {
        fixupNode = target->right;
        rbTransplant(root, target, target->right);
    }
    else if (isSentinel(target->right))
    {
        fixupNode = target->left;
        rbTransplant(root, target, target->left);
    }
    else
    {
        movedNode = rbFindMin(target->right);
        origWasRed = movedNode->isRed;
        fixupNode = movedNode->right;
        if (movedNode->parent == target)
            fixupNode->parent = movedNode;
        else
        {
            rbTransplant(root, movedNode, movedNode->right);
            movedNode->right = target->right;
            movedNode->right->parent = movedNode;
        }
        rbTransplant(root, target, movedNode);
        movedNode->left = target->left;
        movedNode->left->parent = movedNode;
        movedNode->isRed = target->isRed;
    }
    delete target;
    if (!origWasRed)
        rbFixDelete(root, fixupNode);
}
bool rbSearch(RBNode *root, int key)
{
    RBNode *cur = root;
    while (!isSentinel(cur))
    {
        if (key == cur->key)
            return true;
        cur = key < cur->key ? cur->left : cur->right;
    }
    return false;
}

// ===== Structural comparison =====
// Compares the AVL tree and the RB tree structurally (colors ignored).
// An AVL node "matches" if the RB tree has the same key in the same
// position. "mismatchCount" counts AVL nodes where the RB tree has a
// different key at that same position.
// treesMatch stays true only if the two trees are identical everywhere.

int mismatchCount;
bool treesMatch;

void compareStructure(AVLNode *avlNode, RBNode *rbNode)
{
    bool avlIsNull = (avlNode == nullptr);
    bool rbIsNull = isSentinel(rbNode);

    if (avlIsNull && rbIsNull)
        return;
    if (avlIsNull != rbIsNull)
    {
        treesMatch = false;
        return;
    }

    if (avlNode->key != rbNode->key)
    {
        treesMatch = false;
        mismatchCount++;
        // Keep comparing further down even though this pair itself
        // doesn't match — per the spec we still check descendant
        // positions using the AVL tree's shape to guide the walk.
        compareStructure(avlNode->left, rbNode->left);
        compareStructure(avlNode->right, rbNode->right);
        return;
    }
    compareStructure(avlNode->left, rbNode->left);
    compareStructure(avlNode->right, rbNode->right);
}

int main()
{
    rbInitSentinel();
    AVLNode *avlRoot = nullptr;
    RBNode *rbRoot = NIL;

    int q;
    cin >> q;
    while (q--)
    {
        char op;
        int x;
        cin >> op >> x;

        if (op == 'I')
        {
            if (!avlSearch(avlRoot, x))
                avlRoot = avlInsert(avlRoot, x);
            if (!rbSearch(rbRoot, x))
                rbInsert(rbRoot, x);
        }
        else
        {
            if (avlSearch(avlRoot, x))
                avlRoot = avlDelete(avlRoot, x);
            if (rbSearch(rbRoot, x))
                rbDelete(rbRoot, x);
        }

        mismatchCount = 0;
        treesMatch = true;
        compareStructure(avlRoot, isSentinel(rbRoot) ? nullptr : rbRoot);

        // Also catch the case where only one of the two trees is empty.
        bool avlEmpty = (avlRoot == nullptr);
        bool rbEmpty = isSentinel(rbRoot);
        if (avlEmpty != rbEmpty)
            treesMatch = false;

        string avlRootStr = avlRoot ? to_string(avlRoot->key) : "EMPTY";
        string rbRootStr = (!isSentinel(rbRoot)) ? to_string(rbRoot->key) : "EMPTY";

        cout << (treesMatch ? "MATCH" : "NO MATCH") << " " << mismatchCount << " " << avlRootStr << " " << rbRootStr << "\n";
    }

    return 0;
}
