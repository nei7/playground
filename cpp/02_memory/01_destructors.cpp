// Exercise 1 — destructor order.
//
// 1. BEFORE you run it: write down on paper the order in which the
//    "ctor ..." and "dtor ..." lines will appear. Only then `make run-01_destructors`.
// 2. Add `return 0;` inside the inner block. Did the destructors of b and c
//    still run? And the destructor of a?
// 3. Replace `return 0;` with `throw std::runtime_error("x");` (header
//    <stdexcept>). The program will terminate with an error, but check what
//    managed to get printed. Then catch the exception in main with try/catch
//    and compare.
//
// The takeaway: the destructor runs on EVERY exit from a scope.
// RAII (exercise 7) is built on this.

#include <iostream>
#include <string>

struct Loud {
    std::string name;

    explicit Loud(std::string n) : name(n) {
        std::cout << "ctor " << name << '\n';
    }
    ~Loud() {
        std::cout << "dtor " << name << '\n';
    }
};

// 2. yes they still ran all of them
//
int main() {

    Loud a("a");
    {
        Loud b("b");
        Loud c("c");
    }
    std::cout << "end of main\n";

    try {
        throw std::runtime_error("x");
    } catch (std::exception& e) {
    }
}
