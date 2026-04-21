#include <bits/stdc++.h>
#include "skip_list.hpp"
using namespace std;

// We implement a simple interactive harness that supports operations:
// I x: insert x
// Q x: search x -> prints 1 if present else 0
// D x: delete x
// Lines may be many; inputs are integers by default. The judge may ignore this harness
// and only check the header, but we provide a valid main for local testing.

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    SkipList<long long> sl;
    char op;
    long long x;
    // Accept both formats: "I x", "Q x", "D x"; ignore other lines.
    while (cin >> op >> x) {
        if (op == 'I') {
            sl.insert(x);
        } else if (op == 'Q') {
            cout << (sl.search(x) ? 1 : 0) << '\n';
        } else if (op == 'D') {
            sl.deleteItem(x);
        }
    }
    return 0;
}
