#include <bits/stdc++.h>
using namespace std;

// 2-4 TREE (a B-Tree of order 4)
// Every node holds 1-3 keys and 2-4 children.
// All leaves sit at the same depth 

struct TFNode
{
    int keys[3];         // up to 3 keys per node
    int keyCount;        // how many of the 3 slots are actually used
    bool isLeaf;         // true if this node has no children
    TFNode *children[4]; // up to 4 child pointers (nullptr if unused)
    TFNode *parent;      // parent pointer (nullptr for the root)
};

// Creates an empty node (leaf or internal).
TFNode *makeNode(bool leaf)
{
    TFNode *n = new TFNode();
    n->keyCount = 0;
    n->isLeaf = leaf;
    n->parent = nullptr;
    fill(n->children, n->children + 4, nullptr);
    return n;
}

// Search: returns the node containing the key, or nullptr if absent.
TFNode *search(TFNode *cur, int key)
{
    if (!cur)
        return nullptr;
    int i = 0;
    // Walk right until we pass the spot where key would live.
    while (i < cur->keyCount && key > cur->keys[i])
        i++;
    if (i < cur->keyCount && key == cur->keys[i])
        return cur; // found it
    if (cur->isLeaf)
        return nullptr;                   // nowhere else to look
    return search(cur->children[i], key); // keep going down
}

// Splits a full child (a 4-node) before we descend into it.
void splitChild(TFNode *parent, int childIndex, TFNode *child, TFNode *&root)
{
    int midKey = child->keys[1]; // the middle key moves up to parent

    // Build the right half using child's last key.
    TFNode *rightHalf = makeNode(child->isLeaf);
    rightHalf->keys[0] = child->keys[2];
    rightHalf->keyCount = 1;

    // Move child's last two children over to rightHalf (internal nodes only).
    if (!child->isLeaf)
    {
        rightHalf->children[0] = child->children[2];
        rightHalf->children[1] = child->children[3];
        if (rightHalf->children[0])
            rightHalf->children[0]->parent = rightHalf;
        if (rightHalf->children[1])
            rightHalf->children[1]->parent = rightHalf;
    }

    // Shrink child down to just its first key.
    child->keys[1] = child->keys[2] = 0;
    child->keyCount = 1;
    child->children[2] = child->children[3] = nullptr;

    if (!parent)
    {
        // Splitting the root -> build a brand-new root above both halves.
        TFNode *newRoot = makeNode(false);
        newRoot->keys[0] = midKey;
        newRoot->keyCount = 1;
        newRoot->children[0] = child;
        newRoot->children[1] = rightHalf;
        child->parent = newRoot;
        rightHalf->parent = newRoot;
        root = newRoot;
        return;
    }

    // Insert midKey into parent at position childIndex, shifting keys right.
    for (int j = parent->keyCount; j > childIndex; j--)
        parent->keys[j] = parent->keys[j - 1];
    parent->keys[childIndex] = midKey;

    // Shift parent's child pointers right to make room for rightHalf.
    for (int j = parent->keyCount + 1; j > childIndex + 1; j--)
        parent->children[j] = parent->children[j - 1];
    parent->children[childIndex + 1] = rightHalf;

    rightHalf->parent = parent;
    child->parent = parent;
    parent->keyCount++;
}

// Top-down insert
bool insertKey(TFNode *&root, int key)
{
    if (search(root, key))
        return false; // duplicate, reject

    // Empty tree -> single-key leaf root.
    if (!root)
    {
        root = makeNode(true);
        root->keys[0] = key;
        root->keyCount = 1;
        return true;
    }

    // If the root itself is full, split it first.
    if (root->keyCount == 3)
        splitChild(nullptr, 0, root, root);

    TFNode *cur = root;
    while (!cur->isLeaf)
    {
        // Figure out which child to go into.
        int i = 0;
        while (i < cur->keyCount && key > cur->keys[i])
            i++;

        TFNode *child = cur->children[i];
        if (child->keyCount == 3)
        {
            // Child is full -> split it before stepping into it.
            splitChild(cur, i, child, root);
            // After the split, cur->keys[i] is the key that got promoted.
            // Work out whether our target key is now in the left or right half.
            if (key > cur->keys[i])
                i++;
        }
        cur = cur->children[i];
    }

    // cur is a non-full leaf; slot the key into its sorted position.
    int i = cur->keyCount - 1;
    while (i >= 0 && key < cur->keys[i])
    {
        cur->keys[i + 1] = cur->keys[i]; // shift right
        i--;
    }
    cur->keys[i + 1] = key;
    cur->keyCount++;
    return true;
}

void fixDeficientChild(TFNode *&root, TFNode *parent, int childIndex)
{
    TFNode *child = parent->children[childIndex];
    if (child->keyCount >= 2)
        return; // already fine, nothing to do

    TFNode *leftSibling = (childIndex > 0) ? parent->children[childIndex - 1] : nullptr;
    TFNode *rightSibling = (childIndex < parent->keyCount) ? parent->children[childIndex + 1] : nullptr;

    if (leftSibling && leftSibling->keyCount >= 2)
    {
        // Borrow from the LEFT sibling.
        // Shift child's keys right to open up slot 0.
        for (int i = child->keyCount; i > 0; i--)
            child->keys[i] = child->keys[i - 1];
        // The separator key from parent drops down into child.
        child->keys[0] = parent->keys[childIndex - 1];
        // leftSibling's last child moves over to become child's first child.
        if (!child->isLeaf)
        {
            for (int i = child->keyCount + 1; i > 0; i--)
                child->children[i] = child->children[i - 1];
            child->children[0] = leftSibling->children[leftSibling->keyCount];
            if (child->children[0])
                child->children[0]->parent = child;
            leftSibling->children[leftSibling->keyCount] = nullptr;
        }
        // leftSibling's largest key moves up to fill the separator slot.
        parent->keys[childIndex - 1] = leftSibling->keys[leftSibling->keyCount - 1];
        leftSibling->keyCount--;
        child->keyCount++;
    }
    else if (rightSibling && rightSibling->keyCount >= 2)
    {
        // Borrow from the RIGHT sibling.
        // Pull the separator down from parent into the end of child.
        child->keys[child->keyCount] = parent->keys[childIndex];
        if (!child->isLeaf)
        {
            child->children[child->keyCount + 1] = rightSibling->children[0];
            if (child->children[child->keyCount + 1])
                child->children[child->keyCount + 1]->parent = child;
        }
        // rightSibling's smallest key moves up to become the new separator.
        parent->keys[childIndex] = rightSibling->keys[0];
        // Shift rightSibling's keys (and children) left to close the gap.
        for (int i = 0; i < rightSibling->keyCount - 1; i++)
            rightSibling->keys[i] = rightSibling->keys[i + 1];
        if (!rightSibling->isLeaf)
            for (int i = 0; i < rightSibling->keyCount; i++)
                rightSibling->children[i] = rightSibling->children[i + 1];
        rightSibling->keyCount--;
        child->keyCount++;
    }
    else if (rightSibling)
    {
        // Merge: child + parent's separator + rightSibling, all into one node.
        child->keys[child->keyCount] = parent->keys[childIndex];
        child->keyCount++;
        for (int i = 0; i < rightSibling->keyCount; i++)
            child->keys[child->keyCount + i] = rightSibling->keys[i];
        if (!child->isLeaf)
            for (int i = 0; i <= rightSibling->keyCount; i++)
            {
                child->children[child->keyCount + i] = rightSibling->children[i];
                if (child->children[child->keyCount + i])
                    child->children[child->keyCount + i]->parent = child;
            }
        child->keyCount += rightSibling->keyCount;
        // Remove the separator key and the rightSibling pointer from parent.
        for (int i = childIndex; i < parent->keyCount - 1; i++)
            parent->keys[i] = parent->keys[i + 1];
        for (int i = childIndex + 1; i < parent->keyCount; i++)
            parent->children[i] = parent->children[i + 1];
        parent->children[parent->keyCount] = nullptr;
        parent->keyCount--;
        delete rightSibling;
        // If parent (the root) is now empty, child becomes the new root.
        if (parent == root && parent->keyCount == 0)
        {
            root = child;
            child->parent = nullptr;
            delete parent;
        }
    }
    else
    {
        // Merge: leftSibling + parent's separator + child, all into leftSibling.
        leftSibling->keys[leftSibling->keyCount] = parent->keys[childIndex - 1];
        leftSibling->keyCount++;
        for (int i = 0; i < child->keyCount; i++)
            leftSibling->keys[leftSibling->keyCount + i] = child->keys[i];
        if (!leftSibling->isLeaf)
            for (int i = 0; i <= child->keyCount; i++)
            {
                leftSibling->children[leftSibling->keyCount + i] = child->children[i];
                if (leftSibling->children[leftSibling->keyCount + i])
                    leftSibling->children[leftSibling->keyCount + i]->parent = leftSibling;
            }
        leftSibling->keyCount += child->keyCount;
        for (int i = childIndex - 1; i < parent->keyCount - 1; i++)
            parent->keys[i] = parent->keys[i + 1];
        for (int i = childIndex; i < parent->keyCount; i++)
            parent->children[i] = parent->children[i + 1];
        parent->children[parent->keyCount] = nullptr;
        parent->keyCount--;
        delete child;
        if (parent == root && parent->keyCount == 0)
        {
            root = leftSibling;
            leftSibling->parent = nullptr;
            delete parent;
        }
    }
}

// Top-down delete.
bool deleteKey(TFNode *&root, int key)
{
    if (!search(root, key))
        return false; // key isn't in the tree at all

    // Recursive helper that does the actual work.
    function<void(TFNode *, int)> removeFrom = [&](TFNode *cur, int key)
    {
        // Find the position where key lives (or would live) in cur.
        int i = 0;
        while (i < cur->keyCount && key > cur->keys[i])
            i++;

        if (i < cur->keyCount && key == cur->keys[i])
        {
            // Key found right here in cur.
            if (cur->isLeaf)
            {
                // Just slide the remaining keys left to close the gap.
                for (int j = i; j < cur->keyCount - 1; j++)
                    cur->keys[j] = cur->keys[j + 1];
                cur->keyCount--;
                return;
            }

            // Internal node: swap in a predecessor or successor value.
            TFNode *leftChild = cur->children[i];
            TFNode *rightChild = cur->children[i + 1];

            if (leftChild->keyCount >= 2)
            {
                // Use the inorder predecessor (largest key in left subtree).
                TFNode *predNode = leftChild;
                while (!predNode->isLeaf)
                    predNode = predNode->children[predNode->keyCount];
                int predKey = predNode->keys[predNode->keyCount - 1];
                cur->keys[i] = predKey;         // overwrite with predecessor
                removeFrom(leftChild, predKey); // then remove it from the subtree
            }
            else if (rightChild->keyCount >= 2)
            {
                // Use the inorder successor (smallest key in right subtree).
                TFNode *succNode = rightChild;
                while (!succNode->isLeaf)
                    succNode = succNode->children[0];
                int succKey = succNode->keys[0];
                cur->keys[i] = succKey;          // overwrite with successor
                removeFrom(rightChild, succKey); // then remove it from the subtree
            }
            else
            {
                // Both children only have 1 key each -> merge them together
                // along with cur->keys[i], then keep deleting from the merge.
                leftChild->keys[leftChild->keyCount] = cur->keys[i];
                leftChild->keyCount++;
                for (int j = 0; j < rightChild->keyCount; j++)
                    leftChild->keys[leftChild->keyCount + j] = rightChild->keys[j];
                if (!leftChild->isLeaf)
                    for (int j = 0; j <= rightChild->keyCount; j++)
                    {
                        leftChild->children[leftChild->keyCount + j] = rightChild->children[j];
                        if (leftChild->children[leftChild->keyCount + j])
                            leftChild->children[leftChild->keyCount + j]->parent = leftChild;
                    }
                leftChild->keyCount += rightChild->keyCount;
                // Remove the key and rightChild pointer from cur.
                for (int j = i; j < cur->keyCount - 1; j++)
                    cur->keys[j] = cur->keys[j + 1];
                for (int j = i + 1; j < cur->keyCount; j++)
                    cur->children[j] = cur->children[j + 1];
                cur->children[cur->keyCount] = nullptr;
                cur->keyCount--;
                delete rightChild;
                // If cur was the root and is now empty, leftChild becomes the new root.
                if (cur == root && cur->keyCount == 0)
                {
                    root = leftChild;
                    leftChild->parent = nullptr;
                    delete cur;
                }
                removeFrom(leftChild, key); // keep going, now inside the merged node
            }
        }
        else
        {
            // Key isn't in cur -> step into the right child.
            if (cur->isLeaf)
                return; // shouldn't happen if search() found it earlier
            TFNode *child = cur->children[i];
            if (child->keyCount == 1)
            {
                // Make sure child won't end up empty after we remove from it.
                fixDeficientChild(root, cur, i);
                // The tree shape may have changed, so just retry from cur.
                removeFrom(cur, key);
                return;
            }
            removeFrom(child, key);
        }
    };

    removeFrom(root, key);
    return true;
}

// Traversals
void inorderTraversal(TFNode *cur, vector<int> &output)
{
    if (!cur)
        return;
    for (int i = 0; i < cur->keyCount; i++)
    {
        if (!cur->isLeaf)
            inorderTraversal(cur->children[i], output); // left of keys[i]
        output.push_back(cur->keys[i]);
    }
    if (!cur->isLeaf)
        inorderTraversal(cur->children[cur->keyCount], output); // rightmost child
}

void printLevelOrder(TFNode *root)
{
    if (!root)
        return;
    queue<TFNode *> q;
    q.push(root);
    while (!q.empty())
    {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; i++)
        {
            TFNode *cur = q.front();
            q.pop();
            if (i)
                cout << " ";
            // Print the node as [k1|k2|k3]
            cout << "[";
            for (int j = 0; j < cur->keyCount; j++)
            {
                if (j)
                    cout << "|";
                cout << cur->keys[j];
            }
            cout << "]";
            if (!cur->isLeaf)
                for (int j = 0; j <= cur->keyCount; j++)
                    if (cur->children[j])
                        q.push(cur->children[j]);
        }
        cout << "\n";
    }
}

// Sanity check that all the 2-4 tree invariants still hold.
bool validateTree(TFNode *root)
{
    if (!root)
        return true;

    // 1. All leaves must sit at the same depth.
    function<int(TFNode *, int)> leafDepth = [&](TFNode *n, int depth) -> int
    {
        if (!n)
            return -1;
        if (n->isLeaf)
            return depth;
        int expected = -1;
        for (int i = 0; i <= n->keyCount; i++)
        {
            int got = leafDepth(n->children[i], depth + 1);
            if (expected == -1)
                expected = got;
            else if (got != expected)
                return -2; // mismatch found
        }
        return expected;
    };
    if (leafDepth(root, 0) == -2)
        return false;

    // 2. Each node needs 1-3 sorted keys and the right number of children.
    function<bool(TFNode *)> checkNode = [&](TFNode *n) -> bool
    {
        if (!n)
            return true;
        if (n->keyCount < 1 || n->keyCount > 3)
            return false;
        for (int i = 1; i < n->keyCount; i++)
            if (n->keys[i] <= n->keys[i - 1])
                return false; // not sorted
        if (!n->isLeaf)
            for (int i = 0; i <= n->keyCount; i++)
            {
                if (!n->children[i])
                    return false; // missing child
                if (n->children[i]->parent != n)
                    return false; // bad parent pointer
                if (!checkNode(n->children[i]))
                    return false;
            }
        return true;
    };
    return checkNode(root);
}

void printAll(TFNode *root, bool showValidation)
{
    vector<int> inorderVals;
    inorderTraversal(root, inorderVals);
    cout << "INORDER:";
    for (int v : inorderVals)
        cout << " " << v;
    cout << "\nLEVELORDER:\n";
    printLevelOrder(root);
    if (showValidation)
        cout << (validateTree(root) ? "VALID" : "INVALID") << "\n";
    cout << "END\n";
}

int main()
{
    TFNode *root = nullptr;

    // Build the initial tree.
    int n;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        int value;
        cin >> value;
        insertKey(root, value);
    }
    cout << "INITIAL TREE\n";
    printAll(root, false);

    // Search queries.
    int numSearches;
    cin >> numSearches;
    for (int i = 0; i < numSearches; i++)
    {
        int value;
        cin >> value;
        cout << "SEARCH " << value << ": "
             << (search(root, value) ? "Key found" : "Key not found") << "\n";
    }

    // Insert operations.
    int numInserts;
    cin >> numInserts;
    for (int i = 0; i < numInserts; i++)
    {
        int value;
        cin >> value;
        cout << "INSERT " << value << "\n";
        if (!insertKey(root, value))
            cout << "Duplicate key\n";
        else
            printAll(root, true);
    }

    // Delete operations.
    int numDeletes;
    cin >> numDeletes;
    for (int i = 0; i < numDeletes; i++)
    {
        int value;
        cin >> value;
        cout << "DELETE " << value << "\n";
        if (!deleteKey(root, value))
            cout << "Key not found\n";
        else
            printAll(root, true);
    }

    return 0;
}
