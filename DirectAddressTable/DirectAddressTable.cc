/**
 * ------------- Direct Address Table ---------------
 * Direct addressing works well when the universe of
 * keys is reasonably small.
 *
 * There are a finite number of unique keys. When
 * deletions are made keys are reclaimed to be
 * reused.
 *
 * Delete() - Worst Case - O(1)
 * Insert() - Worst Case - O(1)
 * Search() - Worst Case - O(1)
 *
 * A little C & C++ to implementation of CLRS data structures
 * and algorithms as they are laid out in the text.
 *
 * This is not an exercise in best practices. This
 * is an attempt to use C-isms + C++isms in order
 * to use the same semantics the textbook uses.
 *
 * Translating. That's the word. Almost. The goal
 * is to transliterate CLRS.
 *
 * Best,
 *
 * Eric Sanchez @ericdeansanchez on 20180524
 */

#include <array>
#include <iostream>
#include <stack>

#define MAXKEY 12
#define UNIVERSE (MAXKEY + 1)
#define VALID(key) (((key) >= 0 && (key) <= MAXKEY))

struct Entry {
    int satellite;
    int key;
};

class DirectAddressTable {
public:
    std::array<Entry, UNIVERSE> table;
    void Delete(Entry&);
    void Insert(Entry&);
    Entry Search(const Entry&) const;

private:
    int key = 0;
    std::stack<int> reclaimed_keys;
    void reuse_key_on(Entry&);
};

void DirectAddressTable::reuse_key_on(Entry& entry)
{
    auto k = reclaimed_keys.top();
    reclaimed_keys.pop();
    entry.key = k;
    table[entry.key] = entry;
}

void DirectAddressTable::Insert(Entry& entry)
{
    /// if keys have been exhausted--do nothing
    if (key > MAXKEY && reclaimed_keys.empty()) {
        return;
    } else if (key > MAXKEY && !reclaimed_keys.empty()) {
        reuse_key_on(entry);
    } else {
        entry.key = key++;
        table[entry.key] = entry;
    }
}

#define RECLAIM(key) (reclaimed_keys.push((key)))
#define INVALIDATE(key) ((key) = -1)

void DirectAddressTable::Delete(Entry& entry)
{
    /// harmless to delete an entry that is already NULL
    if (VALID(entry.key)) {
        auto k = entry.key;
        RECLAIM(k);
        table[entry.key] = { -1, -1 };
        INVALIDATE(entry.key);
    }
}

Entry DirectAddressTable::Search(const Entry& entry) const
{
    /// return valid entry... or not
    if (VALID(entry.key))
        return table[entry.key];
    return { -1, -1 };
}

int main()
{
    DirectAddressTable D;
    Entry e{ 303 };
    D.Insert(e);
    auto lookup = D.Search(e);
    std::cout << lookup.key << " "
              << lookup.satellite
              << std::endl;
    D.Delete(e);
    lookup = D.Search(e);

    std::cout << lookup.key << " "
              << lookup.satellite
              << std::endl;

    return 0;
}
