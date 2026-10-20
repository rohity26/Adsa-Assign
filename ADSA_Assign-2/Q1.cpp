// Q1: B-Tree with search, top-down insert, top-down delete, level-order display.
// Every non-root node holds MIN..MAX keys (MAX = 2*MIN+1); the root holds 1..MAX keys.
#include <cstdio>
#include <iostream>
#include <vector>
using namespace std;

struct BTreeNode
{
    int *key;                    // MAX entries
    int count;                   // number of keys currently stored
    struct BTreeNode **children; // MAX+1 entries; all null in a leaf
};

int MAXK, MINK; // MAX and MIN from the input

BTreeNode *newNode()
{
    BTreeNode *node = new BTreeNode;
    node->key = new int[MAXK];
    node->children = new BTreeNode *[MAXK + 1];
    node->count = 0;
    for (int i = 0; i <= MAXK; i++)
        node->children[i] = NULL; // unused child pointers must be null
    return node;
}
void freeNode(BTreeNode *node)
{
    delete[] node->key;
    delete[] node->children;
    delete node;
}
bool isLeaf(BTreeNode *node) { return node->children[0] == NULL; }

// (a) search: walk down, at each node find the first key >= target
bool search(BTreeNode *root, int target)
{
    BTreeNode *cur = root;
    while (cur != NULL)
    {
        int i = 0;
        while (i < cur->count && target > cur->key[i])
            i++;
        if (i < cur->count && cur->key[i] == target)
            return true;
        if (isLeaf(cur))
            return false;
        cur = cur->children[i];
    }
    return false;
}

// Split the full child parent->children[idx] (MAX keys) around its middle key key[MIN].
// Left part keeps MIN keys, right part gets MIN keys, the middle key moves up into parent.
void splitChild(BTreeNode *parent, int idx)
{
    BTreeNode *full = parent->children[idx];
    BTreeNode *right = newNode();
    for (int j = 0; j < MINK; j++)
        right->key[j] = full->key[MINK + 1 + j];
    if (!isLeaf(full))
        for (int j = 0; j <= MINK; j++)
        {
            right->children[j] = full->children[MINK + 1 + j];
            full->children[MINK + 1 + j] = NULL;
        }
    right->count = MINK;
    int middle = full->key[MINK];
    full->count = MINK;
    // make room in parent for the promoted key and the new child
    for (int j = parent->count; j > idx; j--)
        parent->key[j] = parent->key[j - 1];
    for (int j = parent->count + 1; j > idx + 1; j--)
        parent->children[j] = parent->children[j - 1];
    parent->key[idx] = middle;
    parent->children[idx + 1] = right;
    parent->count++;
}

void insertNonFull(BTreeNode *node, int k)
{
    if (isLeaf(node))
    {
        int j = node->count - 1;
        while (j >= 0 && node->key[j] > k)
        {
            node->key[j + 1] = node->key[j];
            j--;
        }
        node->key[j + 1] = k;
        node->count++;
        return;
    }
    int i = 0;
    while (i < node->count && k > node->key[i])
        i++;
    if (node->children[i]->count == MAXK)
    { // split a full child before descending into it
        splitChild(node, i);
        if (k > node->key[i])
            i++; // the key may now belong to the new right half
    }
    insertNonFull(node->children[i], k);
}

// (b) insert: caller has already checked that k is absent (duplicates leave the tree unchanged)
BTreeNode *insert(BTreeNode *root, int k)
{
    if (root == NULL)
    {
        root = newNode();
        root->key[0] = k;
        root->count = 1;
        return root;
    }
    if (root->count == MAXK)
    { // split a full root first: tree grows by one level
        BTreeNode *newRoot = newNode();
        newRoot->children[0] = root;
        splitChild(newRoot, 0);
        root = newRoot;
    }
    insertNonFull(root, k);
    return root;
}

// ---------- deletion helpers ----------
// Merge children[idx], key[idx] and children[idx+1] into children[idx]; remove key[idx] from node.
void mergeChildren(BTreeNode *node, int idx)
{
    BTreeNode *left = node->children[idx], *right = node->children[idx + 1];
    left->key[left->count] = node->key[idx];
    for (int j = 0; j < right->count; j++)
        left->key[left->count + 1 + j] = right->key[j];
    if (!isLeaf(left))
        for (int j = 0; j <= right->count; j++)
            left->children[left->count + 1 + j] = right->children[j];
    left->count += right->count + 1;
    for (int j = idx; j < node->count - 1; j++)
        node->key[j] = node->key[j + 1];
    for (int j = idx + 1; j < node->count; j++)
        node->children[j] = node->children[j + 1];
    node->children[node->count] = NULL;
    node->count--;
    freeNode(right);
}
// Child idx takes the separator key; the left sibling's last key moves up into the parent.
void borrowFromLeft(BTreeNode *node, int idx)
{
    BTreeNode *child = node->children[idx], *sib = node->children[idx - 1];
    for (int j = child->count; j > 0; j--)
        child->key[j] = child->key[j - 1];
    if (!isLeaf(child))
        for (int j = child->count + 1; j > 0; j--)
            child->children[j] = child->children[j - 1];
    child->key[0] = node->key[idx - 1];
    if (!isLeaf(child))
    {
        child->children[0] = sib->children[sib->count];
        sib->children[sib->count] = NULL;
    }
    node->key[idx - 1] = sib->key[sib->count - 1];
    child->count++;
    sib->count--;
}
// Child idx takes the separator key; the right sibling's first key moves up into the parent.
void borrowFromRight(BTreeNode *node, int idx)
{
    BTreeNode *child = node->children[idx], *sib = node->children[idx + 1];
    child->key[child->count] = node->key[idx];
    if (!isLeaf(child))
        child->children[child->count + 1] = sib->children[0];
    node->key[idx] = sib->key[0];
    for (int j = 0; j < sib->count - 1; j++)
        sib->key[j] = sib->key[j + 1];
    if (!isLeaf(sib))
    {
        for (int j = 0; j < sib->count; j++)
            sib->children[j] = sib->children[j + 1];
        sib->children[sib->count] = NULL;
    }
    child->count++;
    sib->count--;
}
int maxKey(BTreeNode *node)
{
    while (!isLeaf(node))
        node = node->children[node->count];
    return node->key[node->count - 1];
}
int minKey(BTreeNode *node)
{
    while (!isLeaf(node))
        node = node->children[0];
    return node->key[0];
}

// Top-down deletion: never descend into a child that has only MIN keys.
void deleteFrom(BTreeNode *node, int k)
{
    int i = 0;
    while (i < node->count && k > node->key[i])
        i++;
    if (i < node->count && node->key[i] == k)
    {
        if (isLeaf(node))
        { // key in a leaf: just remove it
            for (int j = i; j < node->count - 1; j++)
                node->key[j] = node->key[j + 1];
            node->count--;
            return;
        }
        BTreeNode *left = node->children[i], *right = node->children[i + 1];
        if (left->count > MINK)
        { // use the predecessor
            int pred = maxKey(left);
            node->key[i] = pred;
            deleteFrom(left, pred);
        }
        else if (right->count > MINK)
        { // use the successor
            int succ = minKey(right);
            node->key[i] = succ;
            deleteFrom(right, succ);
        }
        else
        { // both have MIN keys: merge, then continue
            mergeChildren(node, i);
            deleteFrom(left, k);
        }
        return;
    }
    if (isLeaf(node))
        return; // not present (already checked by caller)
    if (node->children[i]->count == MINK)
    { // top up the child before descending
        if (i > 0 && node->children[i - 1]->count > MINK)
            borrowFromLeft(node, i);
        else if (i < node->count && node->children[i + 1]->count > MINK)
            borrowFromRight(node, i);
        else if (i < node->count)
            mergeChildren(node, i); // merge with the right sibling
        else
        {
            mergeChildren(node, i - 1);
            i--;
        } // otherwise merge with the left sibling
    }
    deleteFrom(node->children[i], k);
}

// (c) deleteKey: caller has already checked that k is present
BTreeNode *deleteKey(BTreeNode *root, int k)
{
    if (root == NULL)
        return NULL;
    deleteFrom(root, k);
    if (root->count == 0)
    { // empty root: replace by its only child
        BTreeNode *old = root;
        root = isLeaf(root) ? NULL : root->children[0];
        freeNode(old);
    }
    return root;
}

// (d) level-order display, one level per line
void display(BTreeNode *root)
{
    if (root == NULL)
    {
        cout << "EMPTY\n"
             << "END\n";
        return;
    }
    vector<BTreeNode *> level(1, root);
    while (!level.empty())
    {
        vector<BTreeNode *> next;
        for (size_t n = 0; n < level.size(); n++)
        {
            if (n)
                cout << ' ';
            BTreeNode *node = level[n];
            for (int j = 0; j < node->count; j++)
            {
                if (j)
                    cout << ',';
                cout << node->key[j];
            }
            if (!isLeaf(node))
                for (int j = 0; j <= node->count; j++)
                    next.push_back(node->children[j]);
        }
        cout << '\n';
        level.swap(next);
    }
    cout << "END\n";
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    int n;
    if (!(cin >> n >> MAXK >> MINK))
        return 0;
    BTreeNode *root = NULL;
    for (int i = 0; i < n; i++)
    {
        int k;
        cin >> k;
        if (!search(root, k))
            root = insert(root, k); // repeated initial keys are ignored silently
    }
    int q;
    cin >> q;
    while (q--)
    {
        int type;
        cin >> type;
        if (type == 4)
        {
            display(root);
            continue;
        }
        int x;
        cin >> x;
        if (type == 1)
            cout << x << (search(root, x) ? " is present\n" : " is not present\n");
        else if (type == 2)
        {
            if (search(root, x))
                cout << x << " is already present\n";
            else
            {
                root = insert(root, x);
                cout << x << " is inserted\n";
            }
        }
        else if (type == 3)
        {
            if (!search(root, x))
                cout << x << " is not present. So it can not be deleted\n";
            else
            {
                root = deleteKey(root, x);
                cout << x << " is deleted\n";
            }
        }
    }
    return 0;
}
