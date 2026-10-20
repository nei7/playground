// Exercise 5 — std::string and std::string_view.
//
// 1. Construction. Build these and print each with its size():
//      std::string a = "hello";        std::string b(5, 'x');
//      std::string c = a + " " + b;    std::string d{a};  d += '!';
//    Then `auto bad = "hello" + " world";` — read the error. A literal is not
//    a std::string, it is a const char array, and arrays don't add. These work:
//      std::string{"hello"} + " world"
//      using namespace std::literals;  auto e = "hello"s;
//    Keep the bad line commented out with the message.
// 2. Indexing: a[0], a.front(), a.back(). Out of range: a.at(100) throws
//    std::out_of_range — catch it (try/catch, header <stdexcept>) and print
//    e.what(). a[100] throws nothing: it is undefined behaviour, the sanitizer
//    reports an overflow, SAN=0 prints some byte. Same lesson as vectors later.
// 3. Searching. std::string::find returns an index, or std::string::npos when
//    not found:
//      auto pos = s.find("needle");
//      if (pos == std::string::npos) ...
//    Print npos. Beginners write `if (pos >= 0)` — why is that always true?
//    Then write `int count_occurrences(const std::string& text, const std::string& word)`
//    using find in a loop. count_occurrences("abababa", "aba") is 2 if matches
//    may not overlap and 3 if they may: decide, document it in a comment,
//    implement your decision, assert it. What should word == "" return?
// 4. Pieces: substr(pos, len) and iterating characters with range-for.
//    Write and test with assert:
//      bool is_palindrome(const std::string& s)          // "kayak" yes, "ab" no, "" yes
//      std::string to_upper(std::string s)       // takes a COPY on purpose, changes and returns it
//      std::string trim(const std::string& s)    // "  a b  " -> "a b", "   " -> ""
//      std::vector<std::string> split(const std::string& s, char sep)
//                                                // "a,b,,c" -> {"a", "b", "", "c"}
//    to_upper: std::toupper (header <cctype>) takes and returns int. The
//    warnings will push you towards static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
//    find out why each of the two casts is there (cppreference on toupper
//    explains the unsigned char one).
//    split: what should split("", ',') and split("a,", ',') return? Decide,
//    document, assert.
// 5. Conversions. Print what each of these does:
//      std::to_string(42)      std::stoi("42")      std::stoi("42abc")
//      std::stoi("abc")        std::stoi("99999999999")
//    Two of them throw (std::invalid_argument, std::out_of_range) — catch
//    them. One silently parses a prefix. std::from_chars (header <charconv>)
//    does neither: it returns a struct with `ec` (std::errc{} on success) and
//    `ptr` (where it stopped). Try it on the same inputs.
// 6. string_view: a non-owning (pointer, length) pair. Write
//      bool starts_with_hello(std::string_view s)
//    (compare s.substr(0, 5) with "hello") and call it with a std::string, a
//    string literal and a.substr(...). All three work, nothing is copied.
//    Rule: read-only string parameters -> std::string_view. Never store or
//    return a string_view to a string that will die (that is 02_memory/03).
// 7. Formatting with std::format / std::println. Predict, then run:
//      std::println("[{:>8}] [{:<8}] [{:^8}]", "r", "l", "c");
//      std::println("{:08.3f} {:x} {:#x} {:#b} {:+}", 3.14159, 255, 255, 5, 7);
//    Then print a three-column table (name, price, quantity) for three
//    products, columns aligned, prices with two decimals.
//
// Takeaway: std::string owns its characters; string_view borrows them.
// find returns npos, not -1. Literals are not strings.
//
// Concepts: https://en.cppreference.com/w/cpp/string/basic_string
//           https://en.cppreference.com/w/cpp/string/basic_string_view
//           https://en.cppreference.com/w/cpp/utility/format/spec
//           https://en.cppreference.com/w/cpp/string/byte/toupper

#include <cassert>
#include <cctype>
#include <charconv>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

int main() {
    // TODO
}
