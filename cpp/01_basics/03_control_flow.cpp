// Exercise 3 — control flow: if, switch, loops.
//
// 1. if with initializer (C++17):
//      if (int r = n % 3; r == 0) ... else if (r == 1) ... else ...
//    r is visible in every branch and nowhere else. Use it to print, for
//    n = 1..10, "divisible", "one over" or "two over". Then try to print r
//    after the whole if/else — read the error, keep it commented out.
// 2. switch. Write `std::string day_name(int d)` returning "Mon".."Sun" for
//    1..7 and "?" otherwise (default). Then deliberately drop one `break` and
//    see what the compiler says (-Wimplicit-fallthrough, part of -Wextra).
//    Where falling through is intentional, write `[[fallthrough]];` and the
//    warning goes away. Next:
//      enum class Light { Red, Yellow, Green };
//      std::string light_name(Light l)
//    with a switch, NO default, and one case left out on purpose: -Wswitch
//    tells you which one. That is why a switch over an enum should not have a
//    default: without one the compiler checks completeness for you. After the
//    switch the compiler still sees a path with no return (an enum variable
//    can hold other values); end the function with `std::unreachable();`
//    (C++23, header <utility>) or a final return and read -Wreturn-type first.
// 3. Loops. Write each as a function and test it with asserts (header
//    <cassert>: assert(expr) aborts the program if expr is false):
//      bool is_prime(int n)             // trial division while i * i <= n; a for loop
//      int collatz_steps(long long n)   // while n != 1: n = even ? n / 2 : 3n + 1
//      int gcd(int a, int b)            // Euclid: while b != 0 ...
//      int digit_sum(int n)             // do/while, so that 0 gives 0 in one pass
//    Checks:
//      is_prime(2) && is_prime(97) && !is_prime(1) && !is_prime(91)
//      collatz_steps(1) == 0 && collatz_steps(6) == 8 && collatz_steps(27) == 111
//      gcd(12, 18) == 6 && gcd(7, 13) == 1 && gcd(0, 5) == 5
//      digit_sum(0) == 0 && digit_sum(1234) == 10
//    gcd(-12, 18): decide what it should return, write it in a comment, make
//    the function do it, add an assert.
// 4. Range-for over `std::vector<int> v{1, 2, 3};`:
//      for (auto x : v)  x *= 2;     // then print v
//      for (auto& x : v) x *= 2;     // then print v
//    Only the second changes the vector: `auto x` is a copy of each element,
//    `auto& x` is the element itself. (The full story is 02_memory/02.)
//    For read-only loops write `for (const auto& x : v)`.
// 5. break / continue. Print the first 10 primes using is_prime, with a
//    `for (int n = 2;; ++n)` loop that has no condition and leaves with break.
//    Then print the numbers 1..20 skipping multiples of 3 with continue.
// 6. The ternary: `std::string parity = n % 2 == 0 ? "even" : "odd";` is
//    fine. Nested ternaries are not fine: write sign(n) -> "negative" /
//    "zero" / "positive" once nested and once with if/else, and decide which
//    one you want to read in a year.
//
// Takeaway: the compiler checks switch completeness and fallthrough for you —
// if you write code in a way that lets it.
//
// Concepts: https://en.cppreference.com/w/cpp/language/if
//           https://en.cppreference.com/w/cpp/language/switch
//           https://en.cppreference.com/w/cpp/language/range-for
//           https://en.cppreference.com/w/cpp/language/attributes/fallthrough

#include <cassert>
#include <print>
#include <string>
#include <utility>
#include <vector>

int main() {
    // TODO
}
