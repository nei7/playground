// Exercise 2 — copy, reference, pointer.
//
// 1. Write three functions that increment a number by 1:
//      void inc_copy(int x);     // by value
//      void inc_ref(int& x);     // by reference
//      void inc_ptr(int* x);     // by pointer
//    Call each one on the same variable and print its value after each call.
//    For inc_ptr you have to pass the address: &n.
// 2. Do `Point b = a;`, change b.x and print a.x. This is a copy, not an alias.
//    Then `Point& r = a;`, change r.x and print a.x again.
// 3. Print sizeof for: Point, int*, Point&, std::string_view, std::string.
//    Think about why string_view is 16 bytes and string is more.
//    (sizeof of a reference is the sizeof of the object it refers to — check it.)
// 4. Try `inc_ptr(nullptr)` and see what the sanitizer says.
//
// Concepts: https://en.cppreference.com/w/cpp/language/reference
//           https://en.cppreference.com/w/cpp/language/pointer

#include <iostream>
#include <string>
#include <string_view>

struct Point {
    int x;
    int y;
};

int main() {
    // TODO
}
