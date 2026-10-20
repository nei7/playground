// Exercise 7 — struct, enum class, member functions, const.
//
// 1. Aggregates:
//      struct Point { int x; int y; };
//      Point a{1, 2};   Point b{.x = 1, .y = 2};   Point c{};   Point d;
//    Print all four. d has indeterminate values (exercise 2, step 1): a struct
//    of ints does not initialize itself. Fix with default member
//    initializers: `int x = 0; int y = 0;` — now `Point d;` is {0, 0}.
//    Always do this for built-in members.
// 2. Values. `Point e = a; e.x = 99;` — a.x is unchanged: e is a copy. Structs
//    are values, like int. Now `a == e` -> error: no operator== (read it).
//    Add inside the struct `bool operator==(const Point&) const = default;`
//    (C++20) and compare again. Print a with std::println — that also fails:
//    write `std::string to_string(Point p)` returning "(1, 2)" for now
//    (formatting your own types properly is a later topic).
// 3. Member functions:
//      struct Rect {
//          Point top_left;
//          int w = 0;
//          int h = 0;
//          int area() const;                 // const: promises not to modify *this
//          bool contains(Point p) const;     // are the edges inside? decide, write it in a comment
//          void move_by(int dx, int dy);     // not const: modifies
//      };
//    Define them outside the struct: `int Rect::area() const { ... }`. Test
//    with asserts. Then make a `const Rect r{...}` and call r.move_by(1, 1):
//    read the error, keep it commented out. Then remove `const` from area()
//    and call r.area(): same error, same reason. Put it back.
//    Rule: every member function that does not modify the object is const.
// 4. enum class:
//      enum class Suit { Clubs, Diamonds, Hearts, Spades };
//    `Suit s = Suit::Hearts; int i = s;` -> error (no implicit conversion;
//    that is the point of the `class`). `std::to_underlying(s)` (C++23,
//    header <utility>) gives 2. Write `std::string_view to_string(Suit)` with a
//    switch and no default (exercise 3, step 2), and `Suit next(Suit)` that
//    wraps Spades -> Clubs. Compare with a plain `enum Old { A, B };` —
//    `int j = A;` compiles and A lands in the enclosing scope. Don't use
//    plain enums in new code.
// 5. Composition:
//      struct Card { Suit suit = Suit::Clubs; int rank = 1; };   // rank 1..13
//    Build a full 52-card deck in a std::vector<Card> with two nested loops
//    (why does the loop over Suit need a static_cast? enums have no ++),
//    print the first and last five cards as "A of Hearts", "10 of Spades"
//    etc., and write `int count_suit(const std::vector<Card>& deck, Suit s)`.
//    Assert it returns 13 for each suit.
// 6. Structured bindings: `auto [x, y] = a;` gives the fields names. Also in
//    range-for: `for (const auto& [suit, rank] : deck)`. Rewrite count_suit
//    with it.
// 7. Layout: print sizeof(Point), sizeof(Rect), sizeof(Card). Then
//      struct P1 { char c; int i; char d; };
//      struct P2 { char c; char d; int i; };
//    and print both sizes. They differ: padding for alignment. Not important
//    yet; know that layout is physical and member order matters.
//
// Takeaway: a struct is a value with named parts. const member functions are
// the only ones callable on a const object, so mark everything that can be.
//
// Concepts: https://en.cppreference.com/w/cpp/language/aggregate_initialization
//           https://en.cppreference.com/w/cpp/language/enum
//           https://en.cppreference.com/w/cpp/language/structured_binding
//           https://en.cppreference.com/w/cpp/language/default_comparisons

#include <cassert>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

int main() {
    // TODO
}
