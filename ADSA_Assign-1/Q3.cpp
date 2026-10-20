#include <bits/stdc++.h>
using namespace std;

// isRed: true = RED, false = BLACK
struct RBNode
{
    int value;
    bool isRed;
    RBNode *left, *right, *parent;
};

bool isNodeBlack(RBNode *n) { return n == nullptr || n->isRed == false; }
bool isNodeRed(RBNode *n) { return n != nullptr && n->isRed == true; }

// Create a new RED node
RBNode *makeNode(int value)
{
    RBNode *n = new RBNode();
    n->value = value;
    n->isRed = true; // newly inserted nodes always start RED
    n->left = n->right = n->parent = nullptr;
    return n;
}

void rotateLeft(RBNode *&root, RBNode *x)
{
    RBNode *y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;

    y->parent = x->parent;
    if (!x->parent)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rotateRight(RBNode *&root, RBNode *y)
{
    RBNode *x = y->left;
    y->left = x->right;
    if (x->right)
        x->right->parent = y;

    x->parent = y->parent;
    if (!y->parent)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    x->right = y;
    y->parent = x;
}

//  Insert fix-up
void fixInsert(RBNode *&root, RBNode *newNode)
{
    while (isNodeRed(newNode->parent))
    { // violation: newNode and parent both RED
        RBNode *grandparent = newNode->parent->parent;
        if (newNode->parent == grandparent->left)
        {
            RBNode *uncle = grandparent->right;
            if (isNodeRed(uncle))
            {
                // Case 1: uncle is RED
                // Push blackness down from grandparent; move newNode up
                newNode->parent->isRed = false;
                uncle->isRed = false;
                grandparent->isRed = true;
                newNode = grandparent; // continue checking from grandparent
            }
            else
            {
                if (newNode == newNode->parent->right)
                {
                    // Case 2: uncle BLACK, newNode is right child (triangle shape)
                    // Convert to Case 3 by rotating parent left
                    newNode = newNode->parent;
                    rotateLeft(root, newNode);
                }
                // Case 3: uncle BLACK, newNode is left child (straight line shape)
                newNode->parent->isRed = false;
                grandparent->isRed = true;
                rotateRight(root, grandparent);
            }
        }
        else
        {
            // Symmetric: parent is RIGHT child of grandparent
            RBNode *uncle = grandparent->left;
            if (isNodeRed(uncle))
            {
                // Case 1 (mirror)
                newNode->parent->isRed = false;
                uncle->isRed = false;
                grandparent->isRed = true;
                newNode = grandparent;
            }
            else
            {
                if (newNode == newNode->parent->left)
                {
                    // Case 2 (mirror)
                    newNode = newNode->parent;
                    rotateRight(root, newNode);
                }
                // Case 3 (mirror)
                newNode->parent->isRed = false;
                grandparent->isRed = true;
                rotateLeft(root, grandparent);
            }
        }
    }
    root->isRed = false; // Rule: root must always be BLACK
}

// Insert a key
bool insertValue(RBNode *&root, int value)
{
    // Standard BST walk to find insertion point
    RBNode *parent = nullptr;
    RBNode *cur = root;
    while (cur)
    {
        parent = cur;
        if (value < cur->value)
            cur = cur->left;
        else if (value > cur->value)
            cur = cur->right;
        else
            return false; // duplicate found — do nothing
    }
    RBNode *newNode = makeNode(value);
    newNode->parent = parent;
    if (!parent)
        root = newNode; // tree was empty
    else if (value < parent->value)
        parent->left = newNode;
    else
        parent->right = newNode;

    fixInsert(root, newNode); // restore RB properties
    return true;
}

// Search
RBNode *findValue(RBNode *root, int value)
{
    while (root)
    {
        if (value == root->value)
            return root;
        else if (value < root->value)
            root = root->left;
        else
            root = root->right;
    }
    return nullptr; // not found
}

// Transplant - replace subtree rooted at target with subtree rooted at replacement
void transplant(RBNode *&root, RBNode *target, RBNode *replacement)
{
    if (!target->parent)
        root = replacement;
    else if (target == target->parent->left)
        target->parent->left = replacement;
    else
        target->parent->right = replacement;
    if (replacement)
        replacement->parent = target->parent; // update replacement's parent (nullptr-safe)
}

// Delete fix-up
void fixDelete(RBNode *&root, RBNode *fixupNode, RBNode *fixupParent)
{
    while (fixupNode != root && isNodeBlack(fixupNode))
    {
        if (fixupNode == fixupParent->left)
        {
            RBNode *sibling = fixupParent->right;
            if (isNodeRed(sibling))
            {
                // Case 1: sibling is RED
                // Rotate to make sibling BLACK, convert to Case 2/3/4
                sibling->isRed = false;
                fixupParent->isRed = true;
                rotateLeft(root, fixupParent);
                sibling = fixupParent->right; // new sibling after rotation
            }
            if (isNodeBlack(sibling->left) && isNodeBlack(sibling->right))
            {
                // Case 2: sibling BLACK with both nephews BLACK
                // Push double-black up by recoloring sibling RED
                sibling->isRed = true;
                fixupNode = fixupParent;
                fixupParent = fixupNode->parent;
            }
            else
            {
                if (isNodeBlack(sibling->right))
                {
                    // Case 3: sibling BLACK, left nephew RED, right nephew BLACK
                    // Rotate sibling right → convert to Case 4
                    if (sibling->left)
                        sibling->left->isRed = false;
                    sibling->isRed = true;
                    rotateRight(root, sibling);
                    sibling = fixupParent->right;
                }
                // Case 4: sibling BLACK, right nephew RED — final fix
                sibling->isRed = fixupParent->isRed;
                fixupParent->isRed = false;
                if (sibling->right)
                    sibling->right->isRed = false;
                rotateLeft(root, fixupParent);
                fixupNode = root; // done; exit loop
            }
        }
        else
        {
            // Symmetric: fixupNode is RIGHT child
            RBNode *sibling = fixupParent->left;
            if (isNodeRed(sibling))
            {
                sibling->isRed = false;
                fixupParent->isRed = true;
                rotateRight(root, fixupParent);
                sibling = fixupParent->left;
            }
            if (isNodeBlack(sibling->right) && isNodeBlack(sibling->left))
            {
                sibling->isRed = true;
                fixupNode = fixupParent;
                fixupParent = fixupNode->parent;
            }
            else
            {
                if (isNodeBlack(sibling->left))
                {
                    if (sibling->right)
                        sibling->right->isRed = false;
                    sibling->isRed = true;
                    rotateLeft(root, sibling);
                    sibling = fixupParent->left;
                }
                sibling->isRed = fixupParent->isRed;
                fixupParent->isRed = false;
                if (sibling->left)
                    sibling->left->isRed = false;
                rotateRight(root, fixupParent);
                fixupNode = root;
            }
        }
    }
    if (fixupNode)
        fixupNode->isRed = false; // ensure final node is BLACK
}

// Delete a key
bool deleteValue(RBNode *&root, int value)
{
    RBNode *target = findValue(root, value);
    if (!target)
        return false; // key not found

    RBNode *movedNode = target;
    bool origColorWasRed = movedNode->isRed; // track color of node physically removed
    RBNode *fixupNode = nullptr;             // node that may inherit double-black
    RBNode *fixupParent = nullptr;           // fixupNode's parent (needed if fixupNode is nullptr)

    if (!target->left)
    {
        // Case A: no left child → splice out target, replace with right child
        fixupNode = target->right;
        fixupParent = target->parent;
        transplant(root, target, target->right);
    }
    else if (!target->right)
    {
        // Case B: no right child → replace with left child
        fixupNode = target->left;
        fixupParent = target->parent;
        transplant(root, target, target->left);
    }
    else
    {
        // Case C: two children → find inorder successor (min of right subtree)
        movedNode = target->right;
        while (movedNode->left)
            movedNode = movedNode->left;    // go to leftmost node in right subtree
        origColorWasRed = movedNode->isRed; // successor's color matters for fixup
        fixupNode = movedNode->right;
        if (movedNode->parent == target)
        {
            fixupParent = movedNode; // fixupNode's parent is movedNode itself (movedNode stays in place)
        }
        else
        {
            fixupParent = movedNode->parent;
            transplant(root, movedNode, movedNode->right); // remove movedNode from current position
            movedNode->right = target->right;
            movedNode->right->parent = movedNode;
        }
        transplant(root, target, movedNode); // put movedNode in target's position
        movedNode->left = target->left;
        movedNode->left->parent = movedNode;
        movedNode->isRed = target->isRed; // successor inherits target's color
    }
    delete target;

    // Fix-up only needed if a BLACK node was removed (black-height violated)
    if (origColorWasRed == false)
        fixDelete(root, fixupNode, fixupParent);
    return true;
}

// Print
string colorLabel(RBNode *n) { return n->isRed ? "R" : "B"; }

void inorderTraversal(RBNode *n, vector<string> &output)
{
    if (!n)
        return;
    inorderTraversal(n->left, output);
    output.push_back(to_string(n->value) + "(" + colorLabel(n) + ")");
    inorderTraversal(n->right, output);
}

void preorderTraversal(RBNode *n, vector<string> &output)
{
    if (!n)
        return;
    output.push_back(to_string(n->value) + "(" + colorLabel(n) + ")");
    preorderTraversal(n->left, output);
    preorderTraversal(n->right, output);
}

void printLevelOrder(RBNode *root)
{
    if (!root)
        return;
    queue<RBNode *> q;
    q.push(root);
    while (!q.empty())
    {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; i++)
        {
            RBNode *cur = q.front();
            q.pop();
            if (i)
                cout << " ";
            cout << cur->value << "(" << colorLabel(cur) << ")";
            if (cur->left)
                q.push(cur->left);
            if (cur->right)
                q.push(cur->right);
        }
        cout << "\n";
    }
}

void printAllTraversals(RBNode *root)
{
    vector<string> inorderVals, preorderVals;
    inorderTraversal(root, inorderVals);
    preorderTraversal(root, preorderVals);
    cout << "INORDER:";
    for (auto &s : inorderVals)
        cout << " " << s;
    cout << "\n";
    cout << "PREORDER:";
    for (auto &s : preorderVals)
        cout << " " << s;
    cout << "\n";
    cout << "LEVELORDER:\n";
    printLevelOrder(root);
    cout << "END\n";
}

int main()
{
    RBNode *root = nullptr; // empty tree

    // Build initial tree
    int n;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        int value;
        cin >> value;
        insertValue(root, value);
    }
    cout << "INITIAL TREE\n";
    printAllTraversals(root);

    // Search operations
    int numSearches;
    cin >> numSearches;
    for (int i = 0; i < numSearches; i++)
    {
        int value;
        cin >> value;
        cout << "SEARCH " << value << ": "
             << (findValue(root, value) ? "Key found" : "Key not found") << "\n";
    }

    // Insert operations
    int numInserts;
    cin >> numInserts;
    for (int i = 0; i < numInserts; i++)
    {
        int value;
        cin >> value;
        cout << "INSERT " << value << "\n";
        if (!insertValue(root, value))
            cout << "Duplicate key\n";
        else
            printAllTraversals(root);
    }

    // Delete operations
    int numDeletes;
    cin >> numDeletes;
    for (int i = 0; i < numDeletes; i++)
    {
        int value;
        cin >> value;
        cout << "DELETE " << value << "\n";
        if (!deleteValue(root, value))
            cout << "Key not found\n";
        else
            printAllTraversals(root);
    }

    return 0;
}
