// Exercise 6 — std::vector, std::array, and the C array they replace.
//
// 1. C array: `int a[5] = {1, 2, 3};` — print all five elements. What are the
//    last two? Then `a[5] = 0;` — one past the end. No compiler error, ASan
//    reports stack-buffer-overflow. Pass the array to `void show(int arr[])`
//    and print sizeof(arr) inside: the array "decayed" to a pointer, the size
//    is gone (-Wsizeof-array-argument tells you so). That is why C arrays
//    are not used in C++ code: they don't know their own size.
// 2. `std::array<int, 5> b{1, 2, 3};` — same memory, but b.size() works, b
//    can be passed and returned by value, compared with ==, and b.at(5)
//    throws. Print it with range-for. Use std::array when the size is a
//    compile-time constant.
// 3. `std::vector<int> v;` — the size is decided at run time. Try, printing v
//    after every step:
//      v.push_back(10); v.push_back(20); v.push_back(30);
//      v.size()  v.empty()  v.front()  v.back()
//      v[1]  v.at(1)  v.at(5) (throws std::out_of_range)  v[5] (UB -> ASan report)
//      v.pop_back()  v.insert(v.begin(), 5)  v.erase(v.begin())  v.clear()
//    Write `void print(const std::vector<int>& v)` producing `[5, 10, 20]` with
//    no trailing comma (the classic "first element" problem — solve it cleanly)
//    and reuse it everywhere below.
// 4. Constructors:
//      std::vector<int> x(5);   std::vector<int> y(5, 7);   std::vector<int> z{5, 7};
//    Print all three. Parentheses and braces mean different things for
//    vector; this bites everyone exactly once.
// 5. The size_t loop:
//      for (int i = 0; i < v.size(); ++i) ...
//    Read the -Wsign-compare warning. .size() returns std::size_t (unsigned);
//    fix: `for (std::size_t i = 0; ...)`. Then write a loop that walks the
//    vector BACKWARDS with `for (std::size_t i = v.size() - 1; i >= 0; --i)`
//    and explain, before running, why it never ends (exercise 1, step 4).
//    Fix it (hint: `i-- > 0`, or count up and index v[v.size() - 1 - i]).
// 6. Functions, tested with assert:
//      int sum(const std::vector<int>& v)                   // {} -> 0
//      int max_of(const std::vector<int>& v)                // {} -> ? decide: assert, or throw
//      std::vector<int> evens(const std::vector<int>& v)    // {1, 2, 3, 4} -> {2, 4}
//      bool contains(const std::vector<int>& v, int x)
//      void reverse_in_place(std::vector<int>& v)           // swap from both ends (std::swap)
//    Then the standard versions: std::accumulate (<numeric>),
//    std::ranges::max_element, std::ranges::find, std::ranges::reverse
//    (<algorithm>). Call each next to your own and assert they agree.
//    max_element and find return iterators — print *it, and check against
//    v.end() for "not found". The library has all of this; you still need to
//    know how it is done.
// 7. 2D: `std::vector<std::vector<int>> grid(3, std::vector<int>(4, 0));`
//    Set grid[1][2] = 7, print it as rows, then write
//      int row_sum(const std::vector<std::vector<int>>& g, std::size_t r)
//    and `int col_sum(..., std::size_t c)`. What happens for grid[3][0]?
//
// Takeaway: std::vector knows its size, owns its memory, and checks bounds
// only when you ask it to (.at). The index type is std::size_t.
//
// Concepts: https://en.cppreference.com/w/cpp/container/vector
//           https://en.cppreference.com/w/cpp/container/array
//           https://en.cppreference.com/w/cpp/language/array
//           https://en.cppreference.com/w/cpp/algorithm/ranges/max_element

#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
#include <print>
#include <stdexcept>
#include <utility>
#include <vector>

int main() {
    // TODO
}
