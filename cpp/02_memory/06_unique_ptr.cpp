// Exercise 6 — std::unique_ptr: exclusive ownership.
//
// 1. `auto p = std::make_unique<Loud>("heap");` See when the dtor gets printed.
// 2. Try `auto q = p;`. Read the compile error in full, it will be
//    long. Find the phrase "deleted function" in it. Learning to read such
//    errors is a real skill.
// 3. `auto r = std::move(p);` Check `p == nullptr` and `r != nullptr`.
//    std::move itself moves nothing: it is a cast that allows
//    r's constructor to steal p's contents.
// 4. Put three unique_ptr<Loud> into a std::vector<std::unique_ptr<Loud>>
//    (you need push_back(std::move(x)) or emplace_back(make_unique...)).
//    Observe the order of destructors when the vector dies.
// 5. Write a function `void take(std::unique_ptr<Loud> p)` and pass it
//    the pointer. Who is the owner now? When will the object die?
//
// Concepts: https://en.cppreference.com/w/cpp/memory/unique_ptr

#include <iostream>
#include <memory>
#include <string>
#include <utility> // std::move
#include <vector>

struct Loud {
    std::string name;

    explicit Loud(std::string n) : name(n) { std::cout << "ctor " << name << '\n'; }
    ~Loud() { std::cout << "dtor " << name << '\n'; }
};

int main() {
    // TODO
}
