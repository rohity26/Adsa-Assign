#include <bits/stdc++.h>
using namespace std;

typedef vector<long long> BigNum;
const long long BASE = 1000000000LL;

BigNum bigFromInt(long long x)
{
    if (x == 0)
        return {0};
    BigNum result;
    while (x > 0)
    {
        result.push_back(x % BASE);
        x /= BASE;
    }
    return result;
}

BigNum bigMultiply(const BigNum &a, const BigNum &b)
{
    BigNum result(a.size() + b.size(), 0);
    for (int i = 0; i < (int)a.size(); i++)
        for (int j = 0; j < (int)b.size(); j++)
        {
            result[i + j] += a[i] * b[j];
            if (result[i + j] >= BASE * BASE)
            { // avoid overflow before we normalize
                result[i + j + 1] += result[i + j] / BASE;
                result[i + j] %= BASE;
            }
        }
    // Sweep through once more to normalize any leftover carries.
    for (int i = 0; i < (int)result.size() - 1; i++)
    {
        result[i + 1] += result[i] / BASE;
        result[i] %= BASE;
    }
    while (result.size() > 1 && result.back() == 0)
        result.pop_back();
    return result;
}

BigNum bigDivide(const BigNum &a, long long divisor)
{
    BigNum result(a.size());
    long long remainder = 0;
    for (int i = (int)a.size() - 1; i >= 0; i--)
    {
        long long cur = a[i] + remainder * BASE;
        result[i] = cur / divisor;
        remainder = cur % divisor;
    }
    while (result.size() > 1 && result.back() == 0)
        result.pop_back();
    return result;
}

BigNum bigAdd(const BigNum &a, const BigNum &b)
{
    BigNum result;
    long long carry = 0;
    for (int i = 0; i < (int)max(a.size(), b.size()) || carry; i++)
    {
        long long sum = carry;
        if (i < (int)a.size())
            sum += a[i];
        if (i < (int)b.size())
            sum += b[i];
        result.push_back(sum % BASE);
        carry = sum / BASE;
    }
    return result;
}

void printBigNum(const BigNum &x)
{
    printf("%lld", x.back());
    for (int i = (int)x.size() - 2; i >= 0; i--)
        printf("%09lld", x[i]);
    printf("\n");
}

struct BSTNode
{
    int val;
    BSTNode *left, *right;
};

BSTNode *bstInsert(BSTNode *root, int key)
{
    if (!root)
    {
        BSTNode *n = new BSTNode{key, nullptr, nullptr};
        return n;
    }
    if (key < root->val)
        root->left = bstInsert(root->left, key);
    else
        root->right = bstInsert(root->right, key);
    return root;
}

int bstSize(BSTNode *root)
{
    if (!root)
        return 0;
    return 1 + bstSize(root->left) + bstSize(root->right);
}

map<pair<int, int>, BigNum> binomialMemo;

BigNum binomialCoeff(int n, int k)
{
    if (k < 0 || k > n)
        return bigFromInt(0);
    if (k == 0 || k == n)
        return bigFromInt(1);
    auto memoKey = make_pair(n, k);
    auto it = binomialMemo.find(memoKey);
    if (it != binomialMemo.end())
        return it->second;
    BigNum result = bigAdd(binomialCoeff(n - 1, k - 1), binomialCoeff(n - 1, k));
    binomialMemo[memoKey] = result;
    return result;
}

map<BSTNode *, BigNum> countMemo;

BigNum countInsertionSequences(BSTNode *root)
{
    if (!root)
        return bigFromInt(1);
    auto it = countMemo.find(root);
    if (it != countMemo.end())
        return it->second;

    int leftSize = bstSize(root->left);
    int rightSize = bstSize(root->right);
    BigNum result = bigMultiply(
        bigMultiply(binomialCoeff(leftSize + rightSize, leftSize), countInsertionSequences(root->left)),
        countInsertionSequences(root->right));
    countMemo[root] = result;
    return result;
}

// Subtracts 1 from a BigNum (used to exclude the given input sequence itself from the final count).
BigNum bigSubtractOne(BigNum a)
{
    int i = 0;
    while (a[i] == 0)
    {
        a[i] = BASE - 1;
        i++;
    }
    a[i]--;
    while (a.size() > 1 && a.back() == 0)
        a.pop_back();
    return a;
}

int main()
{
    int n;
    cin >> n;
    BSTNode *root = nullptr;
    for (int i = 0; i < n; i++)
    {
        int x;
        cin >> x;
        root = bstInsert(root, x);
    }

    BigNum totalSequences = countInsertionSequences(root);
    BigNum answer = bigSubtractOne(totalSequences); // exclude the original permutation

    printBigNum(answer);
    return 0;
}
