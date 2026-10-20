#include <bits/stdc++.h>
using namespace std;

struct BSTNode
{
    int value;
    BSTNode *left;
    BSTNode *right;
    BSTNode *parent;
};

BSTNode *makeNode(int value)
{
    BSTNode *n = new BSTNode();
    n->value = value;
    n->left = n->right = n->parent = nullptr;
    return n;
}

// BST insert.
BSTNode *insertNode(BSTNode *root, int value)
{
    if (!root)
        return makeNode(value);
    if (value == root->value)
        return nullptr; // duplicate found

    if (value < root->value)
    {
        if (!root->left)
        {
            root->left = makeNode(value);
            root->left->parent = root;
        }
        else
        {
            BSTNode *inserted = insertNode(root->left, value);
            if (!inserted)
                return nullptr; // pass the duplicate signal up
        }
    }
    else
    {
        if (!root->right)
        {
            root->right = makeNode(value);
            root->right->parent = root;
        }
        else
        {
            BSTNode *inserted = insertNode(root->right, value);
            if (!inserted)
                return nullptr;
        }
    }
    return root;
}

// BST search — go left/right depending on comparison until found.
BSTNode *findNode(BSTNode *root, int value)
{
    if (!root)
        return nullptr;
    if (value == root->value)
        return root;
    if (value < root->value)
        return findNode(root->left, value);
    return findNode(root->right, value);
}

// Collects values in sorted order (left, node, right).
void inorderTraversal(BSTNode *root, vector<int> &output)
{
    if (!root)
        return;
    inorderTraversal(root->left, output);
    output.push_back(root->value);
    inorderTraversal(root->right, output);
}

// Collects values as (node, left, right)
void preorderTraversal(BSTNode *root, vector<int> &output)
{
    if (!root)
        return;
    output.push_back(root->value);
    preorderTraversal(root->left, output);
    preorderTraversal(root->right, output);
}

// Prints the tree level by level using a simple BFS with a queue.
void printLevelOrder(BSTNode *root)
{
    if (!root)
        return;
    queue<BSTNode *> q;
    q.push(root);
    while (!q.empty())
    {
        int levelSize = q.size();
        vector<int> currentLevel;
        for (int i = 0; i < levelSize; i++)
        {
            BSTNode *cur = q.front();
            q.pop();
            currentLevel.push_back(cur->value);
            if (cur->left)
                q.push(cur->left);
            if (cur->right)
                q.push(cur->right);
        }
        for (int i = 0; i < (int)currentLevel.size(); i++)
        {
            if (i)
                cout << " ";
            cout << currentLevel[i];
        }
        cout << "\n";
    }
}

// Prints inorder, preorder and level-order views of the tree.
void printAllTraversals(BSTNode *root)
{
    vector<int> inorderVals, preorderVals;
    inorderTraversal(root, inorderVals);
    preorderTraversal(root, preorderVals);

    cout << "INORDER:";
    for (int i = 0; i < (int)inorderVals.size(); i++)
        cout << " " << inorderVals[i];

    cout << "\nPREORDER:";
    for (int i = 0; i < (int)preorderVals.size(); i++)
        cout << " " << preorderVals[i];

    cout << "\nLEVELORDER:\n";
    printLevelOrder(root);
    cout << "END\n";
}

// Leftmost node of a subtree = smallest value in that subtree.
BSTNode *findMinNode(BSTNode *root)
{
    while (root->left)
        root = root->left;
    return root;
}

// BST delete.
BSTNode *removeNode(BSTNode *root, int value)
{
    if (!root)
        return nullptr;

    if (value < root->value)
    {
        root->left = removeNode(root->left, value);
        if (root->left)
            root->left->parent = root;
    }
    else if (value > root->value)
    {
        root->right = removeNode(root->right, value);
        if (root->right)
            root->right->parent = root;
    }
    else
    {
        // This is the node we need to remove.
        if (!root->left && !root->right)
        { // leaf node
            delete root;
            return nullptr;
        }
        else if (!root->left)
        { // right node
            BSTNode *child = root->right;
            child->parent = root->parent;
            delete root;
            return child;
        }
        else if (!root->right)
        { // left node
            BSTNode *child = root->left;
            child->parent = root->parent;
            delete root;
            return child;
        }
        else
        {
            // Two children - swap in the inorder successor's value, then remove the successor from the right subtree.
            BSTNode *successor = findMinNode(root->right);
            root->value = successor->value;
            root->right = removeNode(root->right, successor->value);
            if (root->right)
                root->right->parent = root;
        }
    }
    return root;
}

int main()
{
    // Build the initial tree from N values.
    int n;
    cin >> n;
    BSTNode *root = nullptr;
    for (int i = 0; i < n; i++)
    {
        int value;
        cin >> value;
        if (!root)
            root = makeNode(value);
        else
            insertNode(root, value);
    }

    cout << "INITIAL TREE\n";
    printAllTraversals(root);

    // Perform S search queries.
    int numSearches;
    cin >> numSearches;
    for (int i = 0; i < numSearches; i++)
    {
        int value;
        cin >> value;
        BSTNode *found = findNode(root, value);
        cout << "SEARCH " << value << ": " << (found ? "Key found" : "Key not found") << "\n";
    }

    // Perform I insert operations, printing the tree after each successful insert or flag duplicates.
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
            if (!root)
                root = makeNode(value);
            else
                insertNode(root, value);
            printAllTraversals(root);
        }
    }

    // Perform D delete operations, print the tree after each successful delete or flag missing keys.
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
            root = removeNode(root, value);
            printAllTraversals(root);
        }
    }

    return 0;
}
