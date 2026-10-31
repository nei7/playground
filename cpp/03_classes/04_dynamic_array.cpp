// Exercise 4 — the rule of three/five: your own std::vector.
//
// SCENARIO
//   std::vector is the most used data structure in C++, and every
//   interview-level "implement X" starts with it. You write IntArray: a
//   growable array of ints on the heap, with raw new[]/delete[]. Owning raw
//   memory makes the class responsible for copying and freeing it, and the
//   compiler-generated copy gets that wrong. You will see it crash, fix it
//   with the rule of three, make it fast with the rule of five, and measure
//   why vector grows by doubling and not by "one more slot".
//   (In real code you'd use std::vector itself. Writing one once is how you
//   understand what it does for you, and what rule of zero, the next
//   exercise, saves you from.)
//
// YOU WILL LEARN
//   C++
//   - shallow copy: the generated copy ctor copies the POINTER, so two
//     objects share one buffer and both delete[] it.
//   - the rule of three: if a class needs a destructor, it needs a copy
//     constructor and a copy assignment too.
//   - deep copy: allocate your own buffer and copy the elements.
//   - self-assignment and exception safety in operator=, and the
//     copy-and-swap idiom that handles both.
//   - the rule of five: move operations steal the buffer instead of copying it.
//   - const overloading: operator[] for reading a const array and for writing.
//   - a moved-from object must still be safe to destroy and to assign to.
//   Data structures & algorithms
//   - dynamic array: size (elements in use) vs capacity (allocated slots).
//   - growth strategy: doubling gives amortized O(1) push_back; growing by a
//     constant gives O(n) per push_back and O(n^2) in total. You measure it.
//   - insert/erase in the middle shift elements: O(n). Why vector is great
//     at the back and bad at the front.
//
// STEPS
// 1. The data structure. Write
//      class IntArray {
//        public:
//          IntArray() = default;                   // empty: no allocation
//          ~IntArray();                            // delete[] data_
//          void push_back(int value);
//          void pop_back();                        // precondition: !empty() (assert)
//          std::size_t size() const;
//          std::size_t capacity() const;
//          bool empty() const;
//          int& operator[](std::size_t i);              // no bounds check, like vector
//          const int& operator[](std::size_t i) const;  // the same for const arrays
//          int& at(std::size_t i);                      // throws std::out_of_range
//          const int& at(std::size_t i) const;
//        private:
//          void grow(std::size_t new_capacity);    // new block, copy size_ elements, delete[] old
//          int* data_ = nullptr;
//          std::size_t size_ = 0;
//          std::size_t capacity_ = 0;
//      };
//    Invariant (write it above the class): size_ <= capacity_, and data_
//    points to capacity_ ints from new[] (or is nullptr when capacity_ == 0).
//    push_back: if size_ == capacity_, grow(capacity_ == 0 ? 1 : capacity_ * 2),
//    then store the value. In grow use std::copy_n(data_, size_, fresh)
//    from <algorithm>.
//    Tests:
//      IntArray a;
//      for (int i = 0; i < 10; ++i) a.push_back(i * i);
//      assert(a.size() == 10 && a.capacity() == 16);
//      assert(a[3] == 9 && a.at(9) == 81);
//      a.pop_back();
//      assert(a.size() == 9 && a.capacity() == 16);   // pop never shrinks
//    plus: a.at(9) now throws std::out_of_range (catch it, assert it threw).
//    Then: `const IntArray& view = a; int x = view[2];` works, but
//    `view[2] = 5;` doesn't compile. Note the error. Delete the const
//    operator[] for a moment and see `view[2]` fail too; put it back.
//    -> A1: why two operator[]s? What would go wrong with only the const one
//       (returning const int&)? With only the non-const one?
//
// 2. The crash. Now:
//      IntArray original;
//      original.push_back(1);
//      original.push_back(2);
//      IntArray copy = original;
//      copy[0] = 99;
//      std::println("original[0] = {}", original[0]);
//    Predict what it prints, run. Then read the AddressSanitizer report at the
//    end of the block: "attempting double-free". In the report find the two
//    places: the first free is listed under "freed by thread T0 here", the
//    second under the ERROR line. Both are ~IntArray, for which two objects?
//    -> A2: what exactly did the generated copy constructor copy?
//       Why does copy[0] = 99 change original?
//
// 3. Copy constructor: deep copy.
//      IntArray(const IntArray& other);
//    Initializer list: allocate `new int[other.capacity_]`, copy size_ and
//    capacity_; body: std::copy_n. (Careful with capacity_ == 0: new int[0]
//    is legal but you may prefer nullptr; your invariant decides.)
//    Rerun step 2: original[0] stays 1, ASan is quiet.
//
// 4. Copy assignment, naive first. Write exactly this:
//      IntArray& operator=(const IntArray& other) {
//          delete[] data_;
//          int* fresh = new int[other.capacity_];
//          std::copy_n(other.data_, other.size_, fresh);
//          data_ = fresh;
//          size_ = other.size_;
//          capacity_ = other.capacity_;
//          return *this;
//      }
//    `a = b;` works. Now `a = a;` (self-assignment; in real code it hides as
//    `arrays[i] = arrays[j]` with i == j). Run and read the report:
//    heap-use-after-free inside std::copy_n. "freed by" points at the delete[]
//    on the first line: you freed the buffer you were about to copy from.
//    Second, quieter problem: if `new` throws (out of memory), data_ is
//    already deleted but still points there, and the destructor will
//    delete[] it again later.
//    -> A4: write the order of operations that would fix both problems.
//
// 5. Copy-and-swap. Replace operator= with:
//      friend void swap(IntArray& a, IntArray& b) noexcept {
//          std::swap(a.data_, b.data_);
//          std::swap(a.size_, b.size_);
//          std::swap(a.capacity_, b.capacity_);
//      }
//      IntArray& operator=(IntArray other) {   // by VALUE: the copy ctor makes the copy
//          swap(*this, other);                 // take its buffer, give it ours
//          return *this;                       // other's destructor frees our old buffer
//      }
//    Rerun step 4 including `a = a;`: correct, no ASan report.
//    `friend` defined inside the class: a non-member function allowed to see
//    the private members ("hidden friend"; found when called with IntArray
//    arguments). std::swap of three fields can't throw, hence noexcept.
//    -> A5: walk through `a = b;` and say (1) where the new buffer is
//       allocated, (2) where a's old buffer is freed, (3) why a failed
//       allocation now leaves `a` completely untouched.
//
// 6. Rule of five: stop copying temporaries. Add
//      static inline int copy_ctor_calls = 0;     // ++ in the copy ctor
//    Then:
//      IntArray make_sequence(int n);          // returns 0, 1, 2, ..., n - 1
//      std::vector<IntArray> tables;
//      tables.push_back(make_sequence(100'000));
//      tables.push_back(make_sequence(100'000));
//      std::println("copy ctor calls: {}", IntArray::copy_ctor_calls);
//    It prints 3, and each call copied 100 000 ints. The temporary from
//    make_sequence binds to `const IntArray&`, so push_back copies it (twice),
//    and when `tables` grows from 1 to 2 slots it copies the old element too.
//    Add a move constructor:
//      IntArray(IntArray&& other) noexcept
//          : data_{std::exchange(other.data_, nullptr)},
//            size_{std::exchange(other.size_, 0)},
//            capacity_{std::exchange(other.capacity_, 0)} {}
//    (std::exchange from <utility>: sets the left argument to the new value and
//    returns the OLD one, the same trick as in your File class.)
//    Run again: copy ctor calls: 0. You don't need a separate move
//    assignment: operator=(IntArray other) takes its argument by value, so an
//    rvalue is MOVE-constructed into `other` and then swapped. Copy ctor +
//    move ctor + one by-value operator= + destructor + swap is the "rule of
//    four and a half"; the strict rule of five writes both assignments.
//    -> A6: after `IntArray b = std::move(a);` what state is `a` in? Which
//       member functions are still safe to call on it? Does your invariant
//       from step 1 cover this state?
//
// 7. Measure the growth strategy. Add
//      static inline long long element_copies = 0;   // += size_ in grow()
//    Build without sanitizers (they slow every memory access):
//    `make run-04_dynamic_array SAN=0`.
//    Time push_back for n = 10'000, 20'000, 40'000, 80'000:
//      auto t0 = std::chrono::steady_clock::now();
//      ... n push_backs ...
//      auto t1 = std::chrono::steady_clock::now();
//      double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
//    Print n, element_copies (reset per n) and ms in one line each.
//    Then change grow's rule to "one more slot": grow(capacity_ + 1).
//    Reference numbers from the same machine and flags:
//      doubling:  n = 80000   element copies =     131071   ~1 ms
//      +1:        n = 80000   element copies = 3199960000   ~2000 ms
//    With +1, doubling n makes the time about 4x longer (quadratic, O(n^2));
//    with doubling, about 2x (linear, O(n) in total = O(1) per push_back).
//    Write both tables into A7 and restore doubling.
//    -> A7: your tables, and: why is the +1 version quadratic?
//       (How many elements does the k-th push_back copy?)
//
// 8. Insert and erase in the middle.
//      void insert(std::size_t pos, int value);   // pos <= size(); shifts the tail right by one
//      void erase(std::size_t pos);               // pos < size(); shifts the tail left by one
//    Test on {1, 2, 3}: insert(0, 0) -> {0, 1, 2, 3}; insert(4, 9) -> {0, 1, 2, 3, 9};
//    erase(2) -> {0, 1, 3, 9}. Write `bool equals(const IntArray&, std::vector<int>)`
//    to make the asserts short. Hint: shift from the END backwards for insert,
//    or you overwrite the elements you still need.
//    Measure: 20 000 insert(0, x) vs 20 000 push_back (SAN=0).
//    -> A8: what is the cost of insert at the front, and why does std::vector
//       have no push_front? (std::deque, from 06_stl, does.)
//
// DONE WHEN
//   [ ] steps 2 and 4 have their ASan report noted in a comment (what/where)
//   [ ] the final class passes all tests with ASan quiet, zero warnings
//   [ ] step 6 prints copy ctor calls: 0 with the move ctor in place
//   [ ] the two timing tables are in A7
//   [ ] A1-A8 answered
//   [ ] you can explain: shallow vs deep copy, the rule of three, why
//       copy-and-swap is safe, and why doubling gives amortized O(1)
//
// Concepts: https://en.cppreference.com/w/cpp/language/rule_of_three
//           https://en.cppreference.com/w/cpp/language/copy_assignment
//           https://en.cppreference.com/w/cpp/utility/exchange
//           https://en.wikipedia.org/wiki/Dynamic_array
//           https://en.wikipedia.org/wiki/Amortized_analysis

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <print>
#include <stdexcept>
#include <utility>
#include <vector>

int main() {
    // TODO
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A1:
// A2:
// A4:
// A5:
// A6:
// A7:
//   doubling:
//   +1:
// A8:
