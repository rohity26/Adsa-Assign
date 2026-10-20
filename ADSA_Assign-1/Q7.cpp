#include <bits/stdc++.h>
using namespace std;

struct AVLNode
{
    int key, height;
    AVLNode *left, *right;
};

AVLNode *makeNode(int k)
{
    AVLNode *n = new AVLNode();
    n->key = k;
    n->height = 1;
    n->left = n->right = nullptr;
    return n;
}

int getHeight(AVLNode *n) { return n ? n->height : 0; }
void refreshHeight(AVLNode *n)
{
    if (n)
        n->height = 1 + max(getHeight(n->left), getHeight(n->right));
}
int getBalanceFactor(AVLNode *n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

AVLNode *rotateLeft(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *moved = y->left;
    y->left = x;
    x->right = moved;
    refreshHeight(x);
    refreshHeight(y);
    return y;
}
AVLNode *rotateRight(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *moved = x->right;
    x->right = y;
    y->left = moved;
    refreshHeight(y);
    refreshHeight(x);
    return x;
}
AVLNode *rebalance(AVLNode *n)
{
    refreshHeight(n);
    int balance = getBalanceFactor(n);
    if (balance > 1 && getBalanceFactor(n->left) >= 0)
        return rotateRight(n);
    if (balance > 1 && getBalanceFactor(n->left) < 0)
    {
        n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (balance < -1 && getBalanceFactor(n->right) <= 0)
        return rotateLeft(n);
    if (balance < -1 && getBalanceFactor(n->right) > 0)
    {
        n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}
AVLNode *insertKey(AVLNode *root, int key)
{
    if (!root)
        return makeNode(key);
    if (key < root->key)
        root->left = insertKey(root->left, key);
    else if (key > root->key)
        root->right = insertKey(root->right, key);
    return rebalance(root);
}

vector<int> resultKeys;

// Collects every node exactly distance steps below node.
void collectAtDistance(AVLNode *node, int distance)
{
    if (!node || distance < 0)
        return;
    if (distance == 0)
    {
        resultKeys.push_back(node->key);
        return;
    }
    collectAtDistance(node->left, distance - 1);
    collectAtDistance(node->right, distance - 1);
}

// Returns the distance from node down to the target, or -1 if the
// target isn't in this subtree at all. Along the way it also collects
// any distance-K nodes it discovers.
int findTargetAndCollect(AVLNode *node, int target, int K)
{
    if (!node)
        return -1;
    if (node->key == target)
    {
        collectAtDistance(node, K);
        return 0;
    }

    int distFromLeft = findTargetAndCollect(node->left, target, K);
    if (distFromLeft != -1)
    {
        // Target lives in the left subtree, distFromLeft steps down.
        int distFromHere = distFromLeft + 1;
        if (distFromHere == K)
            resultKeys.push_back(node->key);
        else
            collectAtDistance(node->right, K - distFromHere - 1);
        return distFromHere;
    }

    int distFromRight = findTargetAndCollect(node->right, target, K);
    if (distFromRight != -1)
    {
        int distFromHere = distFromRight + 1;
        if (distFromHere == K)
            resultKeys.push_back(node->key);
        else
            collectAtDistance(node->left, K - distFromHere - 1);
        return distFromHere;
    }

    return -1; // target not found on either side
}

int main()
{
    int n;
    cin >> n;
    AVLNode *root = nullptr;
    for (int i = 0; i < n; i++)
    {
        int k;
        cin >> k;
        root = insertKey(root, k);
    }

    int target, K;
    cin >> target >> K;

    // Quick check that the target actually exists in the tree.
    AVLNode *cur = root;
    bool found = false;
    while (cur)
    {
        if (cur->key == target)
        {
            found = true;
            break;
        }
        cur = target < cur->key ? cur->left : cur->right;
    }

    if (!found)
    {
        cout << "EMPTY\n";
        return 0;
    }

    resultKeys.clear();
    findTargetAndCollect(root, target, K);

    if (resultKeys.empty())
    {
        cout << "EMPTY\n";
        return 0;
    }

    sort(resultKeys.begin(), resultKeys.end());
    for (int i = 0; i < (int)resultKeys.size(); i++)
    {
        if (i)
            cout << " ";
        cout << resultKeys[i];
    }
    cout << "\n";
    return 0;
}
