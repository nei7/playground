// Exercise 8 — capstone: read numbers from stdin, print statistics.
//
// No new language features: this is exercises 1–7 in one program. Write it
// so that main is short and the helpers read top-down like a story.
//
// 1. Reading. `double x; while (std::cin >> x) { ... }` reads tokens until
//    end of input or the first token that is not a number. Build and try:
//      make 08_stats
//      echo "1 2 3" | ./bin/08_stats
//      printf "1 2\n3 abc 4\n" | ./bin/08_stats
//    (Or `make run-08_stats < some_file`; make passes stdin through.) In the
//    second case 4 is never read: the stream is in a failed state after
//    "abc". Print std::cin.fail() after the loop to confirm. End of input in a
//    terminal is Ctrl+D.
// 2. Robust reading. Read whole lines with std::getline, split each line into
//    tokens (your split from exercise 5, or std::istringstream from <sstream>
//    with `>>`), convert every token with std::stod inside try/catch (or
//    std::from_chars, <charconv>). Skip bad tokens, but count them and report
//    at the end: "skipped 1 invalid token(s)". Now the second input above
//    reads 1, 2, 3, 4. What about "4abc"? Decide what counts as a number.
// 3. Results. Store the values in std::vector<double>. At the end print
//      count, sum, min, max, mean, median
//    one per line, numbers with two decimals ("{:.2f}"). For the median sort
//    a copy (std::ranges::sort, <algorithm>); for an even count average the
//    two middle values. Empty input: print "no data" and return 1 from main.
//    Check the exit code with `echo $?`.
// 4. Structure. Use these signatures:
//      struct Input { std::vector<double> values; int skipped = 0; };
//      Input read_numbers(std::istream& in);
//      double mean(const std::vector<double>& v);
//      double median(std::vector<double> v);            // by value: sorts its own copy
//      struct Stats { ... };                             // count, sum, min, max, mean, median
//      Stats compute(const std::vector<double>& v);      // precondition: v not empty
//      void print(const Stats& s);
//    read_numbers takes std::istream& so that it can be tested without the
//    keyboard: in main, before using std::cin, call it with a
//    std::istringstream{"1 2 x 3"} and assert values == {1, 2, 3} and
//    skipped == 1. main should then be about ten lines.
// 5. Extras, if you want them: a histogram (one line per bucket, 10 buckets
//    between min and max, one '*' per value); or `int main(int argc, char* argv[])`
//    with an `--ints` flag switching to integer output.
//
// Done when: the three invocations from step 1 (plus one with an empty input:
// `./bin/08_stats < /dev/null`) print correct output, zero warnings, and the
// asserts from step 4 are in place.
//
// Concepts: https://en.cppreference.com/w/cpp/io/basic_istream
//           https://en.cppreference.com/w/cpp/string/basic_string/getline
//           https://en.cppreference.com/w/cpp/io/basic_istringstream
//           https://en.cppreference.com/w/cpp/algorithm/ranges/sort

#include <algorithm>
#include <cassert>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

int main() {
    // TODO
}
