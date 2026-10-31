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

const int& bad() {
    int var = 10;

    return var;
}

int main() {
    // TODO

    // 2.
    // std::string_view v;
    // {
    //     std::string owner = "a fairly long text, definitely more than 15 characters";
    //     v = owner;
    // }
    // std::cout << v << '\n';

    // 3.
    // 02_memory/03_dangling_view.cpp:29:12: runtime error: reference binding to null pointer of
    // type 'const int' #0 0x6174ce0cb255 in bad() 02_memory/03_dangling_view.cpp:29 #1
    // 0x6174ce0cb2a7 in main 02_memory/03_dangling_view.cpp:45 #2 0x79c46f02b390 in
    // __libc_start_call_main
    // (/nix/store/pqyiidz5mi73wv5f91d48s9plcqyj5bz-glibc-2.44-57/lib/libc.so.6+0x2b390) (BuildId:
    // 342d94d33e8a929e8d32ff5226646738d2236e4a) #3 0x79c46f02b4a7 in __libc_start_main_impl
    // (/nix/store/pqyiidz5mi73wv5f91d48s9plcqyj5bz-glibc-2.44-57/lib/libc.so.6+0x2b4a7) (BuildId:
    // 342d94d33e8a929e8d32ff5226646738d2236e4a) #4 0x6174ce0cb0d4 in _start
    // (/home/nei/Projects/playground/cpp/bin/03_dangling_view+0x10d4)

    bad();
}
