#include <bits/stdc++.h>
using namespace std;

struct TreeNode
{
    int val;
    TreeNode *left, *right;
};

// Builds the tree from a level-order list of tokens
TreeNode *buildTree(vector<string> &tokens)
{
    if (tokens.empty() || tokens[0] == "null")
        return nullptr;
    TreeNode *root = new TreeNode{stoi(tokens[0]), nullptr, nullptr};
    queue<TreeNode *> q;
    q.push(root);
    int i = 1;
    while (!q.empty() && i < (int)tokens.size())
    {
        TreeNode *cur = q.front();
        q.pop();
        if (i < (int)tokens.size() && tokens[i] != "null")
        {
            cur->left = new TreeNode{stoi(tokens[i]), nullptr, nullptr};
            q.push(cur->left);
        }
        i++;
        if (i < (int)tokens.size() && tokens[i] != "null")
        {
            cur->right = new TreeNode{stoi(tokens[i]), nullptr, nullptr};
            q.push(cur->right);
        }
        i++;
    }
    return root;
}

long long bestSum = 0;

struct SubtreeInfo
{
    bool isBST;
    long long minVal, maxVal, sum;
};

SubtreeInfo dfs(TreeNode *node)
{
    if (!node)
        return {true, LLONG_MAX, LLONG_MIN, 0};

    SubtreeInfo leftInfo = dfs(node->left);
    SubtreeInfo rightInfo = dfs(node->right);

    long long curSum = (long long)node->val + leftInfo.sum + rightInfo.sum;
    bool curIsBST = leftInfo.isBST && rightInfo.isBST;

    // BST property: everything on the left must be smaller,
    // everything on the right must be bigger, than this node's own value.
    if (node->left && leftInfo.maxVal >= node->val)
        curIsBST = false;
    if (node->right && rightInfo.minVal <= node->val)
        curIsBST = false;

    if (curIsBST)
    {
        bestSum = max(bestSum, curSum);
    }

    return {
        curIsBST,
        min((long long)node->val, leftInfo.minVal),
        max((long long)node->val, rightInfo.maxVal),
        curSum};
}

int main()
{
    int m;
    cin >> m;
    vector<string> tokens(m);
    for (int i = 0; i < m; i++)
        cin >> tokens[i];

    TreeNode *root = buildTree(tokens);
    bestSum = 0;
    dfs(root);
    cout << bestSum << "\n";
    return 0;
}
