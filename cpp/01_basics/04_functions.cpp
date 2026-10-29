// Exercise 4 — functions: declaration vs definition, parameters, overloading,
// defaults, recursion, static locals, namespaces.
//
// 1. Declaration vs definition. Put `int twice(int x);` above main, call it in
//    main, and put the definition `int twice(int x) { return 2 * x; }` BELOW
//    main. Compiles: C++ reads top to bottom and only needs the declaration
//    before the call. Now delete the declaration: read the error ("was not
//    declared in this scope"). Put it back and delete the definition instead:
//    the error now comes from the linker ("undefined reference", "ld
//    returned 1"), not the compiler. Two stages, two kinds of message; learn
//    to tell them apart. Record both messages in comments.
// 2. Parameters. Three ways to take a std::string:
//      void by_value(std::string s);
//      void by_const_ref(const std::string& s);
//      void by_ref(std::string& s);
//    Each appends "!" to s (by_value: to its copy) and prints it. Call each
//    with a std::string variable AND with the literal "abc". One of the six
//    calls does not compile: which, and what does the message say? (A literal
//    becomes a temporary std::string; a non-const reference refuses to bind
//    to a temporary.) After each call print the variable: which calls
//    changed it?
//    Rule for now: input -> `const T&` (cheap types like int: by value),
//    input/output -> `T&`, output -> return value. Reasons in 02_memory.
// 3. Overloading. Write three functions named describe, taking int, double and
//    const std::string&, each returning its parameter's type name as a string.
//    Call describe with: 5, 5.0, "text", 'x', 5L, 5u, 5.0f, true. For each
//    call, write your prediction in a comment BEFORE compiling. Two of the
//    calls are ambiguous: read the errors (they list the candidates), keep
//    them commented out, and decide which overload you would add or remove
//    to make them unambiguous.
// 4. Default arguments: `std::string repeat(const std::string& s, int n = 2);`
//    repeat("ab") == "abab", repeat("ab", 3) == "ababab". If you have both a
//    declaration and a definition, the default goes only on the declaration.
// 5. [[nodiscard]]: mark is_prime (copy it from exercise 3) with it, call it
//    without using the result, read the warning. Mark every function whose
//    result is the whole point of calling it.
// 6. Recursion: `long long factorial(int n)` and `long long fib(int n)`.
//    factorial(20) == 2432902008176640000; factorial(21) overflows long long:
//    run it under the sanitizer and see. fib(40) with naive recursion takes
//    seconds (check with `time ./bin/04_functions` after `make 04_functions
//    SAN=0`); rewrite it as a loop and compare. Recursion is a tool, not a style.
// 7. static local: `int next_id() { static int counter = 0; return ++counter; }`
//    Call it three times, print the results. The variable is initialized once
//    and lives until the program ends. Where else have you seen that pattern?
// 8. Namespaces. Put gcd (from exercise 3) in `namespace math { ... }` and
//    call it as math::gcd. Then write `using namespace std;` at file scope,
//    declare a global `int data = 5;` and print `data` from main: "reference
//    to 'data' is ambiguous" (std::data exists). This is why `using namespace
//    std;` is banned in headers and avoided everywhere. Remove it, keep the
//    error in a comment.
//
// Takeaway: the signature is the contract. Decide how each parameter is
// passed and what is returned before writing the body.
//
// Concepts: https://en.cppreference.com/w/cpp/language/overload_resolution
//           https://en.cppreference.com/w/cpp/language/default_arguments
//           https://en.cppreference.com/w/cpp/language/attributes/nodiscard
//           https://en.cppreference.com/w/cpp/language/namespace

#include <cassert>
#include <print>
#include <string>

void by_value(std::string s) {
    s += '!';
    std::println("{}", s);
}
void by_const_ref(const std::string& s) {
    std::println("{}", s + "!");
}

void by_ref(std::string& s) {
    s += '!';
    std::println("{}", s);
}

int twice(int x);

std::string describe(int n) {
    return "int";
}

std::string describe(double n) {
    return "double";
}

std::string describe(const std::string& n) {
    return "std::string";
}

std::string repeat(const std::string& s, int n = 2) {
    std::string d{};
    for (int i = 0; i < n; i++)
        d.append(s);

    return d;
}

[[nodiscard]]
bool is_prime(int n) {
    if (n < 2)
        return false;

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

long long factorial(int n) {
    if (n <= 1) {
        return n;
    }

    return factorial(n - 1) * n;
}

long long fib(int n) {
    if (n <= 2)
        return n;

    return fib(n - 1) + fib(n - 2);
}

long long factorial_loop(int n) {
    long long factorial = 1;

    for (int i = 1; i <= n; i++) {
        factorial *= i;
    }

    return factorial;
}

long long fib_loop(int n) {
    if (n <= 2)
        return n;

    int a = 1;
    int b = 1;
    int c = 0;

    for (int i = 2; i < n; i++) {
        c = a + b;
        a = b;
        b = c;
    }

    return c;
}

int next_id() {
    static int counter = 0;
    return ++counter;
}

namespace math {

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

}; // namespace math

int data = 5;

int main() {
    // TODO
    // 1.
    std::println("{}", twice(12));

    // 2. only by_ref changes original value by_ref("abc") does not compile
    std::string s{"abc"};

    by_value(s);
    by_value("abc");
    std::println("{}", s);

    by_const_ref(s);
    by_const_ref("abc");
    std::println("{}", s);

    by_ref(s);
    // by_ref("abc");
    std::println("{}", s);

    // 3
    std::println("{}", describe(5));      // int
    std::println("{}", describe(5.0));    // double
    std::println("{}", describe("text")); // string

    // std::println("{}", describe(5L));
    // std::println("{}", describe(5u));

    std::println("{}", describe(5.0f)); // double
    std::println("{}", describe(true)); // int

    // 4
    assert(repeat("ab") == "abab" && repeat("ab", 3) == "ababab");

    // 5.
    is_prime(4);

    // 6.
    // assert(factorial(20) == 2432902008176640000);

    // std::println("{}", fib(20));
    // std::println("{}", factorial(20));

    std::println("{}", fib_loop(20));
    std::println("{}", factorial(20));

    // 7. i have seen that pattern in classes. Methods in classes have access to its properties

    std::println("{}", next_id());
    std::println("{}", next_id());
    std::println("{}", next_id());

    // 8.

    std::println("{}", math::gcd(4, 45));

    std::println("{}", data);
}

int twice(int x) {
    return 2 * x;
}
