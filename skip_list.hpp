#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

// A generic SkipList implementation that only relies on operator< for T.
// Equality is determined via !(a<b) && !(b<a).
// Duplicate inserts are ignored. Deleting non-existent items is a no-op.
// Average time complexity per operation: O(log N).

template<typename T>
class SkipList {
private:
    struct Node {
        T* value;                      // nullptr for head/sentinel
        std::vector<Node*> forward;    // forward pointers per level
        explicit Node(int lvl, T* val = nullptr) : value(val), forward(lvl, nullptr) {}
    };

    static constexpr int MAX_LEVEL = 32;   // sufficient for up to ~2^32 elements
    static constexpr double P = 0.5;       // probability for random level

    Node* head;    // sentinel head node (no value)
    int level;     // current maximum level (1..MAX_LEVEL)

    static inline bool lessThan(const T& a, const T& b) {
        return a < b;
    }
    static inline bool equals(const T& a, const T& b) {
        return !lessThan(a, b) && !lessThan(b, a);
    }

    int randomLevel() {
        int lvl = 1;
        // Use rand() with P=0.5 to decide level heights
        while (lvl < MAX_LEVEL && (std::rand() & 1)) {
            ++lvl;
        }
        return lvl;
    }

    // Find predecessors for each level for the given key; returns the next at level 0 in outNext0
    void findUpdatePath(const T& key, std::vector<Node*>& update, Node*& next0) {
        Node* x = head;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && lessThan(*(x->forward[i]->value), key)) {
                x = x->forward[i];
            }
            update[i] = x;
        }
        next0 = x->forward[0];
    }

public:
    SkipList() : head(new Node(MAX_LEVEL, nullptr)), level(1) {
        // Seed RNG once per process (idempotent if called multiple times across instances)
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            seeded = true;
        }
    }

    ~SkipList() {
        // Delete all nodes (level 0 traversal)
        Node* x = head->forward[0];
        while (x) {
            Node* nxt = x->forward[0];
            delete x->value;
            delete x;
            x = nxt;
        }
        delete head;
    }

    // Insert a value into the skip list. If the value already exists, do nothing.
    void insert(const T& item) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* next0 = nullptr;
        findUpdatePath(item, update, next0);

        if (next0 && equals(*(next0->value), item)) {
            // already exists
            return;
        }

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; ++i) {
                update[i] = head;
            }
            level = newLevel;
        }

        T* stored = new T(item);
        Node* newNode = new Node(newLevel, stored);

        for (int i = 0; i < newLevel; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    // Search for a value in the skip list
    bool search(const T& item) {
        Node* x = head;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[i] && lessThan(*(x->forward[i]->value), item)) {
                x = x->forward[i];
            }
        }
        x = x->forward[0];
        return x && equals(*(x->value), item);
    }

    // Delete a value from the skip list. If the value does not exist in the skip list, do nothing.
    void deleteItem(const T& item) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* next0 = nullptr;
        findUpdatePath(item, update, next0);

        if (!next0 || !equals(*(next0->value), item)) {
            // not found
            return;
        }

        for (int i = 0; i < level; ++i) {
            if (update[i]->forward[i] != next0) break;
            update[i]->forward[i] = next0->forward[i];
        }
        delete next0->value;
        delete next0;

        while (level > 1 && head->forward[level - 1] == nullptr) {
            --level;
        }
    }
};

#endif
