// Exercise 5 — manual heap: new/delete and why we don't use them.
//
// 1. `int* p = new int[10];` without delete. Fill the array, print the sum,
//    exit the program. Read the LeakSanitizer report at the end.
// 2. Add `delete[] p;` — the leak disappears. Add a second `delete[] p;` —
//    read the "attempting double-free" report.
// 3. After delete, try to read p[0]. That is "heap-use-after-free".
// 4. Rewrite the whole thing with std::vector<int>. Both problems disappear
//    without any extra line: vector frees its buffer in its destructor.
// 5. For comparison, do the same with std::unique_ptr<int[]> and
//    std::make_unique<int[]>(10).
//
// Takeaway: new/delete in application code is a smell. Memory is owned by an
// object that frees it in its destructor (RAII).
//
// Concepts: https://en.cppreference.com/w/cpp/language/raii

#include <iostream>
#include <memory>
#include <vector>

int main() {
    // TODO
}
