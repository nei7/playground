// Exercise 4 — std::vector reallocation invalidates pointers.
//
// 1. Reproduce:
//      std::vector<int> v;
//      v.push_back(1);
//      int* first = &v[0];
//      for (int i = 0; i < 1000; ++i) v.push_back(i);
//      std::cout << *first << '\n';
//    Read the sanitizer report. Note that the memory was freed by the vector itself.
// 2. Print v.capacity() after every push_back in the loop and see when
//    it grows. Those are the reallocation moments.
// 3. Fix it in two ways:
//    a) v.reserve(1001) before taking the address,
//    b) keep an index (std::size_t) instead of a pointer.
//    Which fix is fragile and why? What happens when someone changes
//    1000 to 2000 a year from now?
// 4. Bonus: do the same with a std::string_view pointing at an element
//    of a std::vector<std::string>. Same mechanism, different type.
//
// Concepts: https://en.cppreference.com/w/cpp/container/vector (section
//           "Iterator invalidation")

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

int main() {
    // TODO
}
