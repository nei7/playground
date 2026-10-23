// Exercise 2 — initialization, auto, const, constexpr, scope.
//
// 1. Uninitialized:
//      int x;
//      std::println("{}", x);
//    The compiler may or may not warn (-Wuninitialized depends on how the
//    value flows). Run it: garbage, maybe 0, maybe not — and ASan/UBSan stay
//    silent, this is not their job. Valgrind's job it is:
//      make 02_initialization SAN=0 && valgrind ./bin/02_initialization
//    Find "Use of uninitialised value" or "Conditional jump or move depends on
//    uninitialised value(s)" in the output.
//    Rule: never declare a variable without a value. Then fix it and move on.
// 2. Five spellings, same meaning for int:
//      int a = 5;   int b(5);   int c{5};   int d = {5};   int e{};
//    Print all five. e is 0: empty braces mean "value-initialize" (zero for
//    built-in types). Prefer braces: they forbid narrowing (exercise 1, step 8)
//    and `int e{}` is the shortest way to say "zero, on purpose".
// 3. The most vexing parse: `std::string s();` is NOT an empty string.
//    Add it, then call s.size() and read the error. What did the compiler
//    think s is? (GCC even names the problem: -Wvexing-parse.) Keep the two
//    lines commented out with the message. `std::string s{};` or
//    `std::string s;` is what you meant.
// 4. auto deduces the type from the initializer. For each line, predict the
//    type, then confirm with (header <type_traits>):
//      static_assert(std::is_same_v<decltype(a), YourGuess>);
//      auto a = 5;          auto b = 5.0;        auto c = 5.0f;
//      auto d = 5u;         auto e = 5L;         auto f = 'x';
//      auto g = "text";     auto h = std::string{"text"};
//      auto i = true;       auto j = 5 / 2.0;    auto k = 5 / 2;
//    A wrong guess fails to compile with a readable message: that's the point.
//    g is NOT std::string. What is it? (It matters a lot in 02_memory.)
// 5. const: `const int n = 5; n = 6;` — read the error, keep it commented out.
//    `const auto m = 5;` and `auto const m2 = 5;` are the same thing.
//    Rule: everything that does not change after initialization is const.
//    It documents intent, and the compiler enforces it.
// 6. constexpr: `constexpr int kSize = 10; int arr[kSize]{};` compiles.
//    Replace constexpr with const: still compiles — why? Then
//    `int size = 10; int arr2[size];` — -Wpedantic says "ISO C++ forbids
//    variable length array". That is a C feature, not C++: array sizes must be
//    compile-time constants. (Print sizeof(arr) so the array counts as used.)
//    Write `constexpr int square(int x) { return x * x; }` and check it with
//    `static_assert(square(4) == 16);` — the function ran during compilation.
//    Can you still call square(n) with a runtime n? Try.
// 7. Scope and shadowing: declare `int v = 1;` in main, then inside a block
//    `{ int v = 2; ... }` and print v inside and after the block. Read the
//    -Wshadow warning. Shadowing compiles, but it is a bug waiting to happen,
//    so the flags forbid it here. Rename and move on.
//
// Takeaway: `T x{...};` for everything, `const` by default, `constexpr` when
// the value can be computed at compile time.
//
// Concepts: https://en.cppreference.com/w/cpp/language/initialization
//           https://en.cppreference.com/w/cpp/language/auto
//           https://en.cppreference.com/w/cpp/language/constexpr
//           https://en.cppreference.com/w/cpp/language/scope

#include <print>
#include <string>
#include <type_traits>

int main() {}