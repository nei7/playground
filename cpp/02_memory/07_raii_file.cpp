// Exercise 7 — your own RAII class.
//
// 1. Write a class File that:
//    - calls std::fopen(path, mode) in the constructor and prints "open",
//    - calls std::fclose in the destructor and prints "close",
//    - keeps the std::FILE* as a private field.
//    Test it: open a file in a block, write something with std::fputs, leave
//    the block. "close" should appear without any action on your part.
// 2. What happens when fopen returns nullptr (e.g. the path /nonexistent/x)?
//    A constructor cannot return an error, so throw std::runtime_error
//    and catch it in main. This is the same dilemma as Pager in step 6 of cdb.
// 3. Do `File b = a;`. It compiles, but think: two objects, one
//    FILE*, two fclose calls on the same handle. Forbid it:
//      File(const File&) = delete;
//      File& operator=(const File&) = delete;
//    and check that copying is now a compile error.
// 4. Bonus: add a move constructor that takes over the FILE* and leaves
//    nullptr in the source. The destructor must then check for nullptr
//    before fclose. This is the "rule of five" from the ROADMAP.
//
// Concepts: https://en.cppreference.com/w/cpp/language/rule_of_three
//           https://en.cppreference.com/w/cpp/io/c/fopen

#include <cstdio>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

class File {
  public:
    File(const char* path, const char* mode) : file{std::fopen(path, mode)} {
        if (file == nullptr) {
            throw std::runtime_error{"can't open file"};
        }
        std::println("open");
    }

    ~File() {
        if (file != nullptr) {
            std::fclose(file);
            std::println("close");
        }
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&& other) noexcept : file{std::exchange(other.file, nullptr)} {
        std::println("move ctor");
    }

    File& operator=(File&& other) noexcept {
        if (this != &other) {
            if (file != nullptr) {
                std::fclose(file);
            }
            file = std::exchange(other.file, nullptr);
        }
        return *this;
    }

    void write(const char* text) {
        std::fputs(text, file);
    }

  private:
    std::FILE* file = nullptr;
};

int main() {
    // TODO
    {
        try {
            File f("/home/nei/Projets/playground/cpp/README.md", "w+");
            f.write("dss");

            File b = std::move(f);
        } catch (std::runtime_error& e) {
            std::println("{}", e.what());
        }
    }
}
