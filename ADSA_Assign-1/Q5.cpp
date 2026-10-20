#include <bits/stdc++.h>
using namespace std;

// Perfect Hashing: classic two-level scheme.
// Level 1: universal hash h(x) = ((a*x + b) mod p) mod m
// Level 2: for a bucket that ended up with n_j items, use a table of
//          size n_j^2 so that (with a good h2) we get zero collisions.

struct UniversalHashFn {
    long long a, b, p, m;
    long long hash(long long x) const {
        return ((a * x + b) % p + p) % p % m;
    }
};

long long randomInRange(long long lo, long long hi, mt19937_64& rng) {
    return uniform_int_distribution<long long>(lo, hi)(rng);
}

bool isPrime(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

long long nextPrime(long long n) {
    while (!isPrime(n)) n++;
    return n;
}

struct PerfectHashTable {
    int numBuckets;             // first-level table size (m)
    long long prime;            // prime bigger than every key
    UniversalHashFn level1Hash;
    vector<int> bucketSize;               // how many keys landed in each bucket
    vector<UniversalHashFn> level2Hash;   // one hash function per bucket
    vector<vector<pair<int, int>>> level2Table; // level2Table[j] has size bucketSize[j]^2
    // {-1, -1} marks an empty slot

    void build(const vector<int>& keys, mt19937_64& rng) {
        if (keys.empty()) { numBuckets = 0; return; }
        numBuckets = (int)keys.size();
        long long maxKey = *max_element(keys.begin(), keys.end());
        prime = nextPrime(max(maxKey + 1, (long long)numBuckets + 1));

        // Keep re-rolling h1 until no bucket makes the total level-2
        // space blow up (sum of n_j^2 <= 4*m keeps things reasonable).
        while (true) {
            level1Hash.a = randomInRange(1, prime - 1, rng);
            level1Hash.b = randomInRange(0, prime - 1, rng);
            level1Hash.p = prime;
            level1Hash.m = numBuckets;

            bucketSize.assign(numBuckets, 0);
            for (int k : keys) bucketSize[level1Hash.hash(k)]++;

            long long totalSlots = 0;
            for (int count : bucketSize) totalSlots += (long long)count * count;
            if (totalSlots <= 4LL * numBuckets) break; // good enough, stop retrying
        }

        // Now build each bucket's level-2 table.
        level2Hash.resize(numBuckets);
        level2Table.resize(numBuckets);
        vector<vector<int>> bucketContents(numBuckets);
        for (int k : keys) bucketContents[level1Hash.hash(k)].push_back(k);

        for (int j = 0; j < numBuckets; j++) {
            int bucketCount = (int)bucketContents[j].size();
            if (bucketCount == 0) continue;
            int tableSize = bucketCount * bucketCount;
            // Keep re-rolling h2 for this bucket until it's collision-free.
            while (true) {
                level2Hash[j].a = randomInRange(1, prime - 1, rng);
                level2Hash[j].b = randomInRange(0, prime - 1, rng);
                level2Hash[j].p = prime;
                level2Hash[j].m = tableSize;
                level2Table[j].assign(tableSize, make_pair(-1, -1));
                bool collisionFree = true;
                for (int k : bucketContents[j]) {
                    long long slot = level2Hash[j].hash(k);
                    if (level2Table[j][slot].first != -1) { collisionFree = false; break; }
                    level2Table[j][slot] = make_pair(k, k); // storing key as its own value
                }
                if (collisionFree) break;
            }
        }
    }

    // Returns {found, value}
    pair<bool, int> lookup(int key) const {
        if (numBuckets == 0) return make_pair(false, -1);
        int bucket = (int)level1Hash.hash(key);
        int tableSize = (int)level2Table[bucket].size();
        if (tableSize == 0) return make_pair(false, -1);
        long long slot = level2Hash[bucket].hash(key);
        if (slot < 0 || slot >= tableSize) return make_pair(false, -1);
        int storedKey = level2Table[bucket][slot].first;
        int storedVal = level2Table[bucket][slot].second;
        if (storedKey == key) return make_pair(true, storedVal);
        return make_pair(false, -1);
    }
};

void runPerfectHashingTest(const vector<int>& stream, const vector<int>& randomSet, mt19937_64& rng) {
    int n = (int)stream.size();
    int numQueries = (int)ceil(0.05 * n);

    // Distinct values only go into the hash table.
    set<int> dataSet(stream.begin(), stream.end());
    vector<int> distinctKeys(dataSet.begin(), dataSet.end());

    PerfectHashTable table;
    table.build(distinctKeys, rng);

    // Build the test set: first q stream elements unioned with the supplied random set.
    set<int> testSet;
    for (int i = 0; i < min(numQueries, n); i++) testSet.insert(stream[i]);
    for (int r : randomSet) testSet.insert(r);

    int truePositive = 0, falsePositive = 0, falseNegative = 0, trueNegative = 0;
    bool allCorrect = true;

    for (int key : testSet) {
        bool actuallyPresent = dataSet.count(key) > 0;
        pair<bool, int> result = table.lookup(key);
        bool found = result.first;
        int val = result.second;

        if (found) {
            cout << "SEARCH " << key << ": FOUND " << val << "\n";
            if (!actuallyPresent) { falsePositive++; allCorrect = false; }
            else { truePositive++; if (val != key) allCorrect = false; }
        } else {
            cout << "SEARCH " << key << ": NOT FOUND\n";
            if (actuallyPresent) { falseNegative++; allCorrect = false; }
            else trueNegative++;
        }
    }

    cout << "CONFUSION_MATRIX\n";
    cout << "ACTUAL/PREDICTED PRESENT ABSENT\n";
    cout << "PRESENT " << truePositive << " " << falseNegative << "\n";
    cout << "ABSENT " << falsePositive << " " << trueNegative << "\n";
    cout << "RESULT: " << (allCorrect ? "CORRECT" : "INCORRECT") << "\n";
}

int main(int argc, char* argv[]) {

    mt19937_64 rng(42); // fixed seed so runs are repeatable

    if (argc >= 2) {
        ifstream fin(argv[1]);
        if (!fin) {
            cerr << "Could not open file: " << argv[1] << "\n";
            return 1;
        }

        vector<int> stream;
        int x;
        while (fin >> x) stream.push_back(x);
        fin.close();

        int n = (int)stream.size();
        int numQueries = (int)ceil(0.05 * n);

        long long upperBound = max(2000000LL, (long long)n * 5);
        set<int> randomSetBuilder;
        while ((int)randomSetBuilder.size() < numQueries) {
            randomSetBuilder.insert((int)randomInRange(1, upperBound, rng));
        }
        vector<int> randomSet(randomSetBuilder.begin(), randomSetBuilder.end());

        runPerfectHashingTest(stream, randomSet, rng);

    } else {
        int n;
        cin >> n;
        vector<int> stream(n);
        for (int i = 0; i < n; i++) cin >> stream[i];

        int numQueries = (int)ceil(0.05 * n);
        vector<int> randomSet(numQueries);
        for (int i = 0; i < numQueries; i++) cin >> randomSet[i];

        runPerfectHashingTest(stream, randomSet, rng);
    }

    return 0;
}