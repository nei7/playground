// Exercise 3 — dangling string_view.
//
// 1. Reproduce this scenario:
//      std::string_view v;
//      {
//          std::string owner = "a fairly long text, definitely more than 15 characters";
//          v = owner;
//      }
//      std::cout << v << '\n';
//    Run it under the sanitizer and read the WHOLE report (README: how to read it).
//    Find the three places: where it was used, where it was freed, where it was allocated.
// 2. Shorten the text to 5 characters. Did the report change? Why?
//    Hint: look up "small string optimization" (SSO).
// 3. Write a function `const int& bad()` that returns a reference to its
//    local variable. Read the compiler warning. Don't ignore it:
//    such a warning is always a real bug.
// 4. Run the same thing with SAN=0. The program "works"? That is exactly what UB is:
//    no message does not mean the code is correct.
//
// This is the same lesson that the cdb ROADMAP calls the most important one in step 2.

#include <iostream>
#include <string>
#include <string_view>

int main() {
    // TODO
}
