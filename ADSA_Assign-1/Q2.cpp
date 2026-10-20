#include <bits/stdc++.h>
using namespace std;

struct AVLNode
{
    int value, height;
    AVLNode *left;
    AVLNode *right;
    AVLNode *parent;
};

AVLNode *makeNode(int value)
{
    AVLNode *n = new AVLNode();
    n->value = value;
    n->height = 1; // a newly created node is its own leaf, height 1
    n->left = n->right = n->parent = nullptr;
    return n;
}

// Height of a subtree, treating a null pointer as height 0.
int getHeight(AVLNode *n) { return n ? n->height : 0; }

// Recomputes a node height from its children's heights.
void refreshHeight(AVLNode *n)
{
    if (n)
        n->height = 1 + max(getHeight(n->left), getHeight(n->right));
}

// Balance factor = height(left) - height(right).
// Positive  -> left-heavy, Negative -> right-heavy.
int getBalanceFactor(AVLNode *n)
{
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

AVLNode *rotateRight(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *movedSubtree = x->right;

    x->right = y;
    y->left = movedSubtree;

    x->parent = y->parent;
    y->parent = x;
    if (movedSubtree)
        movedSubtree->parent = y;

    refreshHeight(y);
    refreshHeight(x);
    return x;
}

AVLNode *rotateLeft(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *movedSubtree = y->left;
    y->left = x;
    x->right = movedSubtree;

    y->parent = x->parent;
    x->parent = y;
    if (movedSubtree)
        movedSubtree->parent = x;

    refreshHeight(x);
    refreshHeight(y);
    return y;
}

// Re-balances the subtree using LL, LR, RR, RL rotations.
AVLNode *rebalance(AVLNode *n)
{
    refreshHeight(n);
    int balance = getBalanceFactor(n);

    // Left-heavy
    if (balance > 1 && getBalanceFactor(n->left) >= 0)
        return rotateRight(n); // LL case
    if (balance > 1 && getBalanceFactor(n->left) < 0)
    {
        // LR case: first fix the left child, then rotate n
        n->left = rotateLeft(n->left);
        n->left->parent = n;
        return rotateRight(n);
    }

    // Right-heavy
    if (balance < -1 && getBalanceFactor(n->right) <= 0)
        return rotateLeft(n); // RR case
    if (balance < -1 && getBalanceFactor(n->right) > 0)
    {
        // RL case: first fix the right child, then rotate n
        n->right = rotateRight(n->right);
        n->right->parent = n;
        return rotateLeft(n);
    }

    return n; // already balanced
}

// AVL insert.
AVLNode *insertNode(AVLNode *root, int value)
{
    if (!root)
        return makeNode(value);
    if (value == root->value)
        return root; // duplicate

    if (value < root->value)
    {
        root->left = insertNode(root->left, value);
        root->left->parent = root;
    }
    else
    {
        root->right = insertNode(root->right, value);
        root->right->parent = root;
    }
    return rebalance(root);
}

AVLNode *findNode(AVLNode *root, int value)
{
    if (!root)
        return nullptr;
    if (value == root->value)
        return root;
    return value < root->value ? findNode(root->left, value) : findNode(root->right, value);
}

AVLNode *findMinNode(AVLNode *root)
{
    while (root->left)
        root = root->left;
    return root;
}

// AVL delete
AVLNode *deleteNode(AVLNode *root, int value)
{
    if (!root)
        return nullptr;

    if (value < root->value)
    {
        root->left = deleteNode(root->left, value);
    }
    else if (value > root->value)
    {
        root->right = deleteNode(root->right, value);
    }
    else
    {
        if (!root->left || !root->right)
        {
            // Zero or one child
            AVLNode *onlyChild = root->left ? root->left : root->right;
            if (onlyChild)
                onlyChild->parent = root->parent;
            delete root;
            return onlyChild;
        }
        // Two children - replace with inorder successor's value, then remove the successor from the right subtree.
        AVLNode *successor = findMinNode(root->right);
        root->value = successor->value;
        root->right = deleteNode(root->right, successor->value);
    }

    if (root->left)
        root->left->parent = root;
    if (root->right)
        root->right->parent = root;
    return rebalance(root);
}

void inorderTraversal(AVLNode *root, vector<int> &output)
{
    if (!root)
        return;
    inorderTraversal(root->left, output);
    output.push_back(root->value);
    inorderTraversal(root->right, output);
}

void preorderTraversal(AVLNode *root, vector<int> &output)
{
    if (!root)
        return;
    output.push_back(root->value);
    preorderTraversal(root->left, output);
    preorderTraversal(root->right, output);
}

void printLevelOrder(AVLNode *root)
{
    if (!root)
        return;
    queue<AVLNode *> q;
    q.push(root);
    while (!q.empty())
    {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; i++)
        {
            AVLNode *cur = q.front();
            q.pop();
            if (i)
                cout << " ";
            cout << cur->value;
            if (cur->left)
                q.push(cur->left);
            if (cur->right)
                q.push(cur->right);
        }
        cout << "\n";
    }
}

void printAllTraversals(AVLNode *root)
{
    vector<int> inorderVals, preorderVals;
    inorderTraversal(root, inorderVals);
    preorderTraversal(root, preorderVals);

    cout << "INORDER:";
    for (int v : inorderVals)
        cout << " " << v;

    cout << "\nPREORDER:";
    for (int v : preorderVals)
        cout << " " << v;

    cout << "\nLEVELORDER:\n";
    printLevelOrder(root);
    cout << "END\n";
}

int main()
{
    int n;
    cin >> n;
    AVLNode *root = nullptr;
    for (int i = 0; i < n; i++)
    {
        int value;
        cin >> value;
        root = insertNode(root, value);
        if (root)
            root->parent = nullptr; // root has no parent
    }
    cout << "INITIAL TREE\n";
    printAllTraversals(root);

    int numSearches;
    cin >> numSearches;
    for (int i = 0; i < numSearches; i++)
    {
        int value;
        cin >> value;
        cout << "SEARCH " << value << ": " << (findNode(root, value) ? "Key found" : "Key not found") << "\n";
    }

    int numInserts;
    cin >> numInserts;
    for (int i = 0; i < numInserts; i++)
    {
        int value;
        cin >> value;
        cout << "INSERT " << value << "\n";
        if (findNode(root, value))
        {
            cout << "Duplicate key\n";
        }
        else
        {
            root = insertNode(root, value);
            if (root)
                root->parent = nullptr;
            printAllTraversals(root);
        }
    }

    int numDeletes;
    cin >> numDeletes;
    for (int i = 0; i < numDeletes; i++)
    {
        int value;
        cin >> value;
        cout << "DELETE " << value << "\n";
        if (!findNode(root, value))
        {
            cout << "Key not found\n";
        }
        else
        {
            root = deleteNode(root, value);
            if (root)
                root->parent = nullptr;
            printAllTraversals(root);
        }
    }

    return 0;
}
