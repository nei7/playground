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
#include <print>
#include <string>
#include <string_view>

struct Point {
    int x;
    int y;
};

void inc_copy(int x) {
    x++;
}
void inc_ref(int& x) {
    x++;
}

void inc_ptr(int* x) {
    x++;
}

int main() {
    // TODO

    // 1.
    int x = 10;
    inc_copy(x);
    std::println("{}", x);

    inc_ref(x);
    std::println("{}", x);

    inc_ptr(&x);
    std::println("{}", x);

    // 2.
    Point a{10, 11};
    Point b = a;
    b.x = 121;

    std::println("{}", a.x);

    Point& r = a;
    r.x = 12;
    std::println("{}", a.x);

    // 3. string_view has only memory address for the begingi char and it stores also size???
    std::println("{} {} {} {} {}", sizeof(Point), sizeof(int*), sizeof(Point&),
                 sizeof(std::string_view), sizeof(std::string));

    // 4. sanitaizer said nothing program compiled and ran succesfully
    inc_ptr(nullptr);
}
