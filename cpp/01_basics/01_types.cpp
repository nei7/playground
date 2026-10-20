// Exercise 1 — fundamental types: sizes, limits, overflow, conversions.
//
// 1. Print sizeof for: bool, char, short, int, long, long long, float, double,
//    long double and a pointer (void*). The standard fixes only the ORDER
//    (char <= short <= int <= long <= long long), not the sizes. Then print
//    sizeof for std::int8_t, std::int32_t, std::int64_t, std::uint64_t
//    (header <cstdint>) — these are fixed. Which would you use in a file format?
// 2. Print std::numeric_limits<T>::min() and max() (header <limits>) for int,
//    unsigned, long long and double. Then double's lowest(). Why is min() of
//    double a tiny POSITIVE number and not the most negative one?
// 3. Signed overflow is undefined behaviour:
//      int big = std::numeric_limits<int>::max();
//      big = big + 1;
//    Print big. Run as usual: UBSan reports "signed integer overflow". Run with
//    SAN=0: it "works" and prints a negative number. Both are the same bug;
//    the second one just hides.
// 4. Unsigned arithmetic is NOT undefined, it wraps around (modulo 2^N):
//      unsigned u = 0;
//      u = u - 1;
//    Print u. No report from anyone. Well-defined, and still almost always a bug.
// 5. Mixing signed and unsigned:
//      int i = -1;
//      unsigned u = 1;
//      std::println("{}", i < u);
//    Predict the output BEFORE running, then read the -Wsign-compare warning.
//    The int is converted to unsigned, not the other way round. This is the
//    most common bug in loops over .size() — exercise 6 comes back to it.
// 6. Integer division and modulo: print 7 / 2, -7 / 2, 7 % 2, -7 % 2,
//    7.0 / 2, 7 / 2.0. Truncation goes toward zero; the sign of % follows the
//    dividend. Then `int zero = 0; std::println("{}", 7 / zero);` — UBSan
//    catches that too (SAN=0: the process is killed by SIGFPE).
// 7. Floating point is not exact:
//      double a = 0.1 + 0.2;
//      std::println("{} {}", a == 0.3, a);
//      std::println("{:.20f}", a);
//    Comparing doubles with == is almost always wrong. Write
//    `bool nearly_equal(double x, double y)` using an epsilon and use it.
//    What does 1e16 + 1.0 - 1e16 give, and why?
// 8. Narrowing: `int n = 3.99;` compiles (with a -Wconversion warning),
//    `int m{3.99};` does not. Try both, read both messages, keep the second
//    one commented out with the error text. Then `char c = 300;` — read the
//    warning, print c as an int (static_cast<int>(c)). Braces forbid
//    narrowing, which is why exercise 2 recommends them.
//
// Takeaway: `int` is not "a number", it is 32 bits with rules. Know the rules.
//
// Concepts: https://en.cppreference.com/w/cpp/language/types
//           https://en.cppreference.com/w/cpp/types/numeric_limits
//           https://en.cppreference.com/w/cpp/language/implicit_conversion

#include <cstdint>
#include <limits>
#include <print>

int main() {
    // 1. I would use fixed-width types in a file format
    std::println("bool: {}", sizeof(bool));
    std::println("char: {}", sizeof(char));
    std::println("short: {}", sizeof(short));
    std::println("int: {}", sizeof(int));
    std::println("long: {}", sizeof(long));
    std::println("long long: {}", sizeof(long long));
    std::println("float: {}", sizeof(float));
    std::println("double: {}", sizeof(double));
    std::println("long double: {}", sizeof(long double));
    std::println("pointer: {}", sizeof(void*));

    std::println("int8_t: {}", sizeof(std::int8_t));
    std::println("int32_t: {}", sizeof(std::int32_t));
    std::println("int64_t: {}", sizeof(std::int64_t));
    std::println("uint64_t: {}", sizeof(std::uint64_t));

    std::println("\n\n");

    // 2. Because a diffrent bit is used for a sign
    std::println("int min: {}", std::numeric_limits<int>::min());
    std::println("int max: {}", std::numeric_limits<int>::max());

    std::println("unsigned min: {}", std::numeric_limits<unsigned>::min());
    std::println("unsigned max: {}", std::numeric_limits<unsigned>::max());

    std::println("long long min: {}", std::numeric_limits<long long>::min());
    std::println("long long max: {}", std::numeric_limits<long long>::max());

    std::println("double min: {}", std::numeric_limits<double>::min());
    std::println("double max: {}", std::numeric_limits<double>::max());
    std::println("double min: {}", std::numeric_limits<double>::lowest());

    // 3.
    int big = std::numeric_limits<int>::max();
    big = big + 1;
    std::println("big: {}", big);
}
