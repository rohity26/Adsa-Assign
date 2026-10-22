// Q1 : B-tree with search, insert, delete and level-order display.
//
// Rules of this B-tree (MAX = 2*MIN + 1):
//   * every node keeps its keys sorted
//   * every node except the root has between MIN and MAX keys
//   * the root has between 1 and MAX keys (an empty tree is just a NULL root)
//   * a node with k keys has k+1 children (a leaf has none) and all leaves are at the same depth
//
// Example with MIN = 1, MAX = 3 (each node holds 1 to 3 keys):
//
//                 [ 20 ]                      <- root: 1 key, 2 children
//                /      \      (two children)
//         [ 5 10 ]      [ 30 40 50 ]          <- leaves
//
// The two big ideas of this assignment are both "fix the problem BEFORE going down":
//   insert : if a node on the way down is full (MAX keys), split it first,
//            so there is always room for the new key and we never have to climb back up.
//   delete : if the child we are about to enter has only MIN keys, give it an extra key first
//            (borrow from a sibling or merge with a sibling), so it can safely lose one key.

#include <iostream>
#include <vector>
using namespace std;

struct BTreeNode {
    int *key;                      // sorted keys, room for MAX entries
    int count;                     // how many keys are really stored
    struct BTreeNode **children;   // room for MAX + 1 child pointers (all NULL in a leaf)
};

int MAXK;   // MAX from the input
int MINK;   // MIN from the input

// Create an empty node: no keys, every child pointer NULL.
BTreeNode *newNode() {
    BTreeNode *node = new BTreeNode;
    node->key = new int[MAXK];
    node->children = new BTreeNode *[MAXK + 1];
    node->count = 0;
    for (int i = 0; i <= MAXK; i++) {
        node->children[i] = NULL;      // unused child pointers must be NULL
    }
    return node;
}

// Give the memory of one node back (used when two nodes are merged or an empty root is dropped).
void freeNode(BTreeNode *node) {
    delete[] node->key;
    delete[] node->children;
    delete node;
}

// A node is a leaf when it has no first child.
bool isLeaf(BTreeNode *node) {
    return node->children[0] == NULL;
}

// ---------------------------------------------------------------------------------------------
// (a) search
// At every node find the first key that is >= the target.
// Example: node [10 20 30], target 25 -> the first key >= 25 is 30 (index 2), so the key is not in this
// node and can only be in children[2], the subtree that lies between 20 and 30.
// ---------------------------------------------------------------------------------------------
bool search(BTreeNode *root, int target) {
    BTreeNode *current = root;
    while (current != NULL) {
        int i = 0;
        while (i < current->count && target > current->key[i]) {
            i++;
        }
        if (i < current->count && current->key[i] == target) {
            return true;                       // found it in this node
        }
        if (isLeaf(current)) {
            return false;                      // nowhere left to look
        }
        current = current->children[i];        // go down into the correct child
    }
    return false;                              // empty tree
}

// ---------------------------------------------------------------------------------------------
// Split the FULL child parent->children[idx] (it holds MAX keys) into two nodes of MIN keys each.
// The middle key (index MIN) moves up into the parent.
//
// Example with MIN = 2, MAX = 5. Parent [ 50 ], full child (idx 0) = [ 1 2 3 4 5 ]
//
//        [ 50 ]                          [ 3 50 ]
//        /                    ==>        /   |
//   [ 1 2 3 4 5 ]                  [ 1 2 ] [ 4 5 ]
//
// left part keeps the keys before the middle, the new right part gets the keys after it.
// An internal node also hands its upper MIN+1 children to the right part.
// ---------------------------------------------------------------------------------------------
void splitChild(BTreeNode *parent, int idx) {
    BTreeNode *fullChild = parent->children[idx];
    BTreeNode *rightPart = newNode();
    bool childIsLeaf = isLeaf(fullChild);

    // keys after the middle go to the right part
    for (int j = 0; j < MINK; j++) {
        rightPart->key[j] = fullChild->key[MINK + 1 + j];
    }
    // children after the middle key go to the right part as well
    if (!childIsLeaf) {
        for (int j = 0; j <= MINK; j++) {
            rightPart->children[j] = fullChild->children[MINK + 1 + j];
            fullChild->children[MINK + 1 + j] = NULL;
        }
    }
    rightPart->count = MINK;

    int middleKey = fullChild->key[MINK];
    fullChild->count = MINK;                   // the left part keeps the first MIN keys

    // open a gap in the parent for the middle key (at idx) and the new child (at idx + 1)
    for (int j = parent->count; j > idx; j--) {
        parent->key[j] = parent->key[j - 1];
    }
    for (int j = parent->count + 1; j > idx + 1; j--) {
        parent->children[j] = parent->children[j - 1];
    }
    parent->key[idx] = middleKey;
    parent->children[idx + 1] = rightPart;
    parent->count++;
}

// Insert k into the subtree of 'node', which is guaranteed NOT to be full.
void insertNonFull(BTreeNode *node, int k) {
    if (isLeaf(node)) {
        // Leaf: shift bigger keys one place to the right and drop k into the gap.
        // Example: node [10 30 40], k = 20 -> [10 20 30 40]
        int j = node->count - 1;
        while (j >= 0 && node->key[j] > k) {
            node->key[j + 1] = node->key[j];
            j--;
        }
        node->key[j + 1] = k;
        node->count++;
        return;
    }

    // Internal node: choose the child that k belongs to.
    int i = 0;
    while (i < node->count && k > node->key[i]) {
        i++;
    }
    // If that child is full, split it BEFORE going down.
    if (node->children[i]->count == MAXK) {
        splitChild(node, i);
        // The middle key moved up into node->key[i]; k may now belong to the new right half.
        if (k > node->key[i]) {
            i++;
        }
    }
    insertNonFull(node->children[i], k);
}

// ---------------------------------------------------------------------------------------------
// (b) insert  (the caller has already checked that k is not in the tree)
// If the ROOT is full we split it first. This is the only way the tree grows taller:
//
//    [ 10 20 30 ]   insert 25        [ 20 ]
//     (MAX = 3)         ==>         /      \     (tree is now one level taller)
//                               [ 10 ]    [ 25 30 ]
// ---------------------------------------------------------------------------------------------
BTreeNode *insert(BTreeNode *root, int k) {
    if (root == NULL) {                        // empty tree: the new key becomes the root
        root = newNode();
        root->key[0] = k;
        root->count = 1;
        return root;
    }
    if (root->count == MAXK) {
        BTreeNode *newRoot = newNode();
        newRoot->children[0] = root;           // old root becomes the only child
        splitChild(newRoot, 0);
        root = newRoot;
    }
    insertNonFull(root, k);
    return root;
}

// ---------------------------------------------------------------------------------------------
// Deletion helpers
// ---------------------------------------------------------------------------------------------

// Merge children[idx], the separator node->key[idx] and children[idx + 1] into ONE node (children[idx]).
// Example with MIN = 1: parent [ 20 ], children [ 10 ] and [ 30 ]
//
//        [ 20 ]                    [ ]   (parent loses the key 20 and one child)
//        /    \          ==>        |
//    [ 10 ]  [ 30 ]            [ 10 20 30 ]
void mergeChildren(BTreeNode *node, int idx) {
    BTreeNode *leftChild = node->children[idx];
    BTreeNode *rightChild = node->children[idx + 1];
    bool childrenAreLeaves = isLeaf(leftChild);

    // the separator comes down to the end of the left child
    leftChild->key[leftChild->count] = node->key[idx];
    // then all keys of the right child follow
    for (int j = 0; j < rightChild->count; j++) {
        leftChild->key[leftChild->count + 1 + j] = rightChild->key[j];
    }
    // an internal node also takes over the children of the right child
    if (!childrenAreLeaves) {
        for (int j = 0; j <= rightChild->count; j++) {
            leftChild->children[leftChild->count + 1 + j] = rightChild->children[j];
        }
    }
    leftChild->count = leftChild->count + rightChild->count + 1;

    // remove the separator key and the right child pointer from the parent
    for (int j = idx; j < node->count - 1; j++) {
        node->key[j] = node->key[j + 1];
    }
    for (int j = idx + 1; j < node->count; j++) {
        node->children[j] = node->children[j + 1];
    }
    node->children[node->count] = NULL;
    node->count--;

    freeNode(rightChild);
}

// Child number idx is short of keys; its LEFT sibling has a spare one.
// The separator key comes down into the child and the sibling's biggest key goes up (like passing a baton).
// Example (MIN = 1): parent [ 20 40 ], left sibling [ 5 10 ], child [ 25 ] (idx = 1)
//
//      [ 20 40 ]                [ 10 40 ]
//      /   |                    /    |
//  [5 10] [25]       ==>      [5]  [20 25]
void borrowFromLeft(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx - 1];
    bool childIsLeaf = isLeaf(child);

    // make room at the front of the child
    for (int j = child->count; j > 0; j--) {
        child->key[j] = child->key[j - 1];
    }
    if (!childIsLeaf) {
        for (int j = child->count + 1; j > 0; j--) {
            child->children[j] = child->children[j - 1];
        }
    }
    child->key[0] = node->key[idx - 1];                         // separator comes down
    if (!childIsLeaf) {
        child->children[0] = sibling->children[sibling->count]; // sibling's last child moves over
        sibling->children[sibling->count] = NULL;
    }
    node->key[idx - 1] = sibling->key[sibling->count - 1];      // sibling's last key goes up
    child->count++;
    sibling->count--;
}

// Mirror image: the RIGHT sibling lends its smallest key.
// Example (MIN = 1): parent [ 20 40 ], child [ 25 ] (idx = 1), right sibling [ 50 60 ]
//
//      [ 20 40 ]                [ 20 50 ]
//        |   \                    |    \      (right sibling gave away its first key)
//      [25] [50 60]     ==>    [25 40] [60]
void borrowFromRight(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];
    bool childIsLeaf = isLeaf(child);

    child->key[child->count] = node->key[idx];                  // separator comes down to the end
    if (!childIsLeaf) {
        child->children[child->count + 1] = sibling->children[0];
    }
    node->key[idx] = sibling->key[0];                           // sibling's first key goes up

    for (int j = 0; j < sibling->count - 1; j++) {              // close the gap in the sibling
        sibling->key[j] = sibling->key[j + 1];
    }
    if (!childIsLeaf) {
        for (int j = 0; j < sibling->count; j++) {
            sibling->children[j] = sibling->children[j + 1];
        }
        sibling->children[sibling->count] = NULL;
    }
    child->count++;
    sibling->count--;
}

// Largest key of a subtree = keep going to the last child, then take the last key (the predecessor).
int maxKey(BTreeNode *node) {
    while (!isLeaf(node)) {
        node = node->children[node->count];
    }
    return node->key[node->count - 1];
}

// Smallest key of a subtree = keep going to the first child, then take the first key (the successor).
int minKey(BTreeNode *node) {
    while (!isLeaf(node)) {
        node = node->children[0];
    }
    return node->key[0];
}

// ---------------------------------------------------------------------------------------------
// Delete k from the subtree of 'node'. The caller guarantees that 'node' has more than MIN keys
// (or is the root) and that k really is somewhere below.
// ---------------------------------------------------------------------------------------------
void deleteFrom(BTreeNode *node, int k) {
    // find the first key >= k
    int i = 0;
    while (i < node->count && k > node->key[i]) {
        i++;
    }

    // ---- Case A: k is stored in this node ----
    if (i < node->count && node->key[i] == k) {
        if (isLeaf(node)) {
            // A1: leaf -> just remove it.  Example [10 20 30], delete 20 -> [10 30]
            for (int j = i; j < node->count - 1; j++) {
                node->key[j] = node->key[j + 1];
            }
            node->count--;
            return;
        }

        BTreeNode *leftChild = node->children[i];
        BTreeNode *rightChild = node->children[i + 1];
        if (leftChild->count > MINK) {
            // A2: left child can spare a key -> replace k by its predecessor, then delete the predecessor below
            int predecessor = maxKey(leftChild);
            node->key[i] = predecessor;
            deleteFrom(leftChild, predecessor);
        } else if (rightChild->count > MINK) {
            // A3: otherwise the right child can spare one -> use the successor
            int successor = minKey(rightChild);
            node->key[i] = successor;
            deleteFrom(rightChild, successor);
        } else {
            // A4: both children have exactly MIN keys -> merge them with k, then delete k from the merged node
            mergeChildren(node, i);
            deleteFrom(leftChild, k);
        }
        return;
    }

    // ---- Case B: k is not in this node, so it must be inside children[i] ----
    if (isLeaf(node)) {
        return;                                // not present (the caller already checked, so this never happens)
    }
    if (node->children[i]->count == MINK) {
        // The child is at the minimum: make it bigger BEFORE going down.
        // Order required by the assignment: left sibling, then right sibling, then merge.
        bool leftCanLend = (i > 0) && (node->children[i - 1]->count > MINK);
        bool rightCanLend = (i < node->count) && (node->children[i + 1]->count > MINK);
        if (leftCanLend) {
            borrowFromLeft(node, i);
        } else if (rightCanLend) {
            borrowFromRight(node, i);
        } else if (i < node->count) {
            mergeChildren(node, i);            // merge with the right sibling if there is one
        } else {
            mergeChildren(node, i - 1);        // otherwise merge with the left sibling
            i--;                               // the merged node now sits at position i - 1
        }
    }
    deleteFrom(node->children[i], k);
}

// ---------------------------------------------------------------------------------------------
// (c) deleteKey  (the caller has already checked that k is in the tree)
// After the deletion the root may have lost its last key. Then:
//   * a leaf root becomes NULL (empty tree)
//   * an internal root is replaced by its only child (the tree gets one level shorter)
// ---------------------------------------------------------------------------------------------
BTreeNode *deleteKey(BTreeNode *root, int k) {
    if (root == NULL) {
        return NULL;
    }
    deleteFrom(root, k);
    if (root->count == 0) {
        BTreeNode *oldRoot = root;
        if (isLeaf(root)) {
            root = NULL;
        } else {
            root = root->children[0];
        }
        freeNode(oldRoot);
    }
    return root;
}

// ---------------------------------------------------------------------------------------------
// (d) display, level by level (breadth first).
// Keys of one node are separated by commas, nodes of one level by a space, one level per line.
// Example: the tree  [45] -> [20] [60 70]  prints
//      45
//      20 60,70
//      END
// ---------------------------------------------------------------------------------------------
void display(BTreeNode *root) {
    if (root == NULL) {
        cout << "EMPTY\n";
        cout << "END\n";
        return;
    }
    vector<BTreeNode *> currentLevel;
    currentLevel.push_back(root);
    while (!currentLevel.empty()) {
        vector<BTreeNode *> nextLevel;
        for (size_t n = 0; n < currentLevel.size(); n++) {
            if (n > 0) {
                cout << ' ';
            }
            BTreeNode *node = currentLevel[n];
            for (int j = 0; j < node->count; j++) {
                if (j > 0) {
                    cout << ',';
                }
                cout << node->key[j];
            }
            if (!isLeaf(node)) {
                for (int j = 0; j <= node->count; j++) {
                    nextLevel.push_back(node->children[j]);
                }
            }
        }
        cout << '\n';
        currentLevel.swap(nextLevel);
    }
    cout << "END\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (!(cin >> n >> MAXK >> MINK)) {
        return 0;
    }

    // build the tree from the initial keys; repeated keys are ignored without any output
    BTreeNode *root = NULL;
    for (int i = 0; i < n; i++) {
        int k;
        cin >> k;
        if (!search(root, k)) {
            root = insert(root, k);
        }
    }

    int q;
    cin >> q;
    while (q--) {
        int type;
        cin >> type;
        if (type == 4) {
            display(root);
            continue;
        }
        int x;
        cin >> x;
        if (type == 1) {
            if (search(root, x)) {
                cout << x << " is present\n";
            } else {
                cout << x << " is not present\n";
            }
        } else if (type == 2) {
            // searching first means a duplicate insert leaves the tree completely unchanged
            if (search(root, x)) {
                cout << x << " is already present\n";
            } else {
                root = insert(root, x);
                cout << x << " is inserted\n";
            }
        } else if (type == 3) {
            // same for deleting an absent key: top-down deletion would otherwise rearrange nodes
            if (!search(root, x)) {
                cout << x << " is not present. So it can not be deleted\n";
            } else {
                root = deleteKey(root, x);
                cout << x << " is deleted\n";
            }
        }
    }
    return 0;
}
