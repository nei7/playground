// Exercise 3 — the six special member functions: who gets called, and when.
//
// SCENARIO
//   A text editor is slow. The profiler says most of the time goes into
//   copying documents: somewhere, big objects are copied millions of times.
//   To find those places you need to look at a line of C++ and know which
//   special member function it calls. Here you build a document type that
//   reports every call, train that eye on a dozen typical lines, and then
//   audit a piece of "slow editor" code down to zero copies. On the way you
//   measure how std::vector grows, which is the textbook example of
//   amortized O(1).
//
// YOU WILL LEARN
//   C++
//   - the six special members and their exact signatures: default ctor,
//     copy ctor, copy assignment, move ctor, move assignment, destructor.
//   - construction vs assignment: `Doc b = a;` constructs a NEW object (copy
//     ctor, despite the `=`); `b = a;` assigns to an existing one.
//   - copy elision: returning a local by value normally calls nothing at all.
//   - by value copies, by const& doesn't; `for (auto x : v)` copies every element.
//   - std::vector moves its elements when it grows ONLY if the move
//     constructor is noexcept; otherwise it copies them.
//   - declaring a destructor (or a copy operation) silently removes the
//     compiler-generated move operations: moves turn into copies. This is
//     why the "rule of five" exists.
//   - static data members: one variable shared by the whole class.
//   Data structures & algorithms
//   - dynamic array growth: capacity doubles, so n push_backs cause fewer
//     than n element moves in total: amortized O(1) per push_back.
//
// The WHY of moving (rvalues, what std::move really does) is 04_move. Here
// you only learn to recognize WHICH function runs.
//
// STEPS
// 1. Write `class Doc` (a stand-in for a big document) with a private
//    `std::string name_ = "default";`, a getter
//    `const std::string& name() const`, and all six special members, each
//    printing exactly one line:
//      Doc()                                  "  Doc() <name>"
//      explicit Doc(std::string name)         "  ctor <name>"
//      Doc(const Doc& other)                  "  COPY ctor <name>"
//      Doc& operator=(const Doc& other)       "  COPY assign <name>"
//      Doc(Doc&& other) noexcept              "  move ctor <name>"
//      Doc& operator=(Doc&& other) noexcept   "  move assign <name>"
//      ~Doc()                                 "  dtor <name>"
//    COPY is in capitals because copies are what you hunt for.
//    The copy members copy name_. The move members take it:
//    `name_ = std::move(other.name_);` and then set
//    `other.name_ = "moved-from";` so the destructor output shows which
//    objects were emptied. Both assignments end with `return *this;`.
//    Add two counters to the public part:
//      static inline int copies = 0;   // ++ in both copy members
//      static inline int moves = 0;    // ++ in both move members
//    `static`: one counter for the whole class, not one per object. Read it
//    as Doc::copies. `inline` lets you define it right here in the class.
//
// 2. The prediction table. Add above main:
//      Doc make_doc(std::string name) {
//          Doc d{std::move(name)};
//          return d;
//      }
//      void by_value(Doc d) { std::println("  in by_value {}", d.name()); }
//      void by_const_ref(const Doc& d) { std::println("  in by_const_ref {}", d.name()); }
//    and paste into main, inside its own { } block:
//      std::println("--- 1");  Doc a{"a"};
//      std::println("--- 2");  Doc b = a;
//      std::println("--- 3");  Doc c{a};
//      std::println("--- 4");  b = c;
//      std::println("--- 5");  Doc d = std::move(a);
//      std::println("--- 6");  c = std::move(d);
//      std::println("--- 7");  Doc e;
//      std::println("--- 8");  Doc f = make_doc("f");
//      std::println("--- 9");  by_value(b);
//      std::println("--- 10"); by_const_ref(b);
//      std::println("--- 11"); by_value(Doc{"tmp"});
//      std::println("--- end of block");
//    BEFORE running, write next to each line `// predict: ...` with the
//    exact lines you expect, including the destructors at the end of the
//    block and their order. Then run and compare.
//    Things to notice: line 2 has `=` but is construction; line 8 prints a
//    single "ctor f" (no copy, no move: the compiler builds `d` directly in
//    f's place, "named return value optimization"); line 11 builds the
//    temporary directly into the parameter.
//    -> A2: list every line where your prediction was wrong, and why.
//
// 3. Returning with std::move makes it WORSE. Add
//      Doc make_doc_moved(std::string name) { Doc d{std::move(name)}; return std::move(d); }
//    and call it. Read the warning [-Wpessimizing-move]: the std::move
//    blocks the optimization from step 2 and forces an extra move ctor (see
//    it in the output). Keep it commented out with the warning noted.
//    Rule: `return local;`, never `return std::move(local);`.
//
// 4. std::vector and noexcept. Paste in a new { } block:
//      std::vector<Doc> docs;
//      std::println("--- push 1"); docs.push_back(Doc{"v1"});
//      std::println("--- push 2"); docs.push_back(Doc{"v2"});
//      std::println("--- push 3"); docs.push_back(Doc{"v3"});
//      std::println("--- emplace"); docs.emplace_back("v4");
//      std::println("--- end");
//    Predict, run, compare. At "push 2" and "push 3" the vector is full,
//    allocates a bigger block and MOVES the old elements over (02_memory/04
//    showed why pointers into it die at that moment). emplace_back builds
//    the element in place: no temporary at all.
//    Now delete `noexcept` from the move CONSTRUCTOR only and run again: the
//    old elements are COPIED. std::vector promises that if something throws
//    while it grows, the vector stays as it was. A move that might throw
//    halfway would leave some elements already gutted, so vector only moves
//    if the move ctor promises not to throw. Put noexcept back.
//    -> A4: in one sentence, why does vector copy instead of move when the
//       move constructor isn't noexcept? Then: what does
//       docs.reserve(4) before the pushes change in the output? (try it)
//
// 5. How a vector grows: amortized O(1). In a new block:
//      Doc::moves = 0;
//      std::vector<Doc> many;
//      std::size_t last_capacity = many.capacity();
//      for (int i = 0; i < 1000; ++i) {
//          many.emplace_back("x");
//          if (many.capacity() != last_capacity) { ... print i, old and new capacity ... }
//      }
//    The printing of "ctor x"/"dtor x" floods the output: add a
//    `static inline bool quiet = false;` to Doc, skip the printing in every
//    member when it's true, and set it around this block.
//    Print each capacity change and finally Doc::moves.
//    Expected: the capacity goes 1, 2, 4, 8, ..., 1024 and moves is 1023.
//    1000 push_backs caused 1023 moves in total, about one per push_back,
//    even though one single push_back (at capacity 512) moved 512 elements.
//    That's what "amortized O(1)" means: expensive steps are rare enough
//    that the AVERAGE cost per operation is constant. Exercise 4 shows what
//    happens with a different growth rule.
//    -> A5: with doubling, why is the total number of moves always < 2n?
//       (Hint: 1 + 2 + 4 + ... + 512 = ?)
//
// 6. The destructor trap. Paste above main:
//      struct Editor {
//          Doc doc{"draft"};
//      };
//    and in main: `Editor e1; Editor e2 = std::move(e1);` -> "move ctor draft".
//    Now add a "harmless" destructor for logging to Editor:
//      ~Editor() { std::println("  closing editor"); }
//    Run again: the move became a COPY. No error, no warning. Declaring a
//    destructor stops the compiler from generating move operations, and
//    std::move(e1) falls back to the copy constructor. In a real program this
//    is a silent slowdown that appears in a commit that "only added logging".
//    Fix it the rule-of-five way: keep the destructor and declare the other
//    four as `= default`:
//      Editor(const Editor&) = default;
//      Editor& operator=(const Editor&) = default;
//      Editor(Editor&&) = default;
//      Editor& operator=(Editor&&) = default;
//    Now `Editor e1;` stops compiling: read why (declaring a copy ctor also
//    removes the default ctor) and add `Editor() = default;`.
//    -> A6: fill in from your experiments: if you declare a destructor, the
//       compiler does/doesn't generate: copy ctor ___, move ctor ___.
//       If you declare a copy ctor: default ctor ___, move ctor ___.
//
// 7. Audit. This is the "slow editor" code. Paste it above main:
//      std::vector<Doc> open_all(std::vector<std::string> names) {
//          std::vector<Doc> result;
//          for (auto name : names) {
//              Doc d{name};
//              result.push_back(d);
//          }
//          return result;
//      }
//      void print_titles(std::vector<Doc> docs) {
//          for (auto d : docs) {
//              std::println("{}", d.name());
//          }
//      }
//    and in main:
//      Doc::copies = 0;
//      auto opened = open_all({"a.txt", "b.txt", "c.txt"});
//      print_titles(opened);
//      std::println("copies: {}", Doc::copies);
//    It prints "copies: 9". Find all nine (mark each with a comment), then
//    change the two functions so it prints "copies: 0" with the same titles.
//    Tools: const& parameters, `const auto&` in loops, emplace_back (or
//    push_back(std::move(d))), and reserve. Leave the original versions
//    commented out above your fixed ones.
//    -> A7: list the copies you found and the fix for each.
//
// DONE WHEN
//   [ ] your predictions are written next to every line of step 2 and 4
//   [ ] step 5 prints the capacity sequence and moves: 1023
//   [ ] step 6 shows a move again after the rule-of-five fix
//   [ ] step 7 prints copies: 0
//   [ ] A2-A7 answered, zero warnings
//   [ ] you can look at `T x = y;`, `x = y;`, `f(x)`, `return x;` and
//       say which special member runs
//
// Concepts: https://en.cppreference.com/w/cpp/language/rule_of_three
//           https://en.cppreference.com/w/cpp/language/copy_elision
//           https://en.cppreference.com/w/cpp/language/static#Static_data_members
//           https://en.cppreference.com/w/cpp/container/vector/push_back
//           https://en.wikipedia.org/wiki/Amortized_analysis

#include <cstddef>
#include <print>
#include <string>
#include <utility>
#include <vector>

int main() {
    // TODO
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A2:
// A4:
// A5:
// A6:
// A7:
