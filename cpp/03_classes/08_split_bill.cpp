// Exercise 8 — capstone: splitting shared expenses after a trip.
//
// SCENARIO
//   Four friends on a weekend trip take turns paying: one pays for dinner
//   for everybody, another for a train ticket for two. At the end the app
//   tells everyone their balance and who should pay whom, in a few
//   transfers. Input is text, one expense per line:
//      <payer> <amount> <participant> <participant> ...
//   "ala 120.00 ala bartek celina" = Ala paid 120.00 for herself, Bartek and
//   Celina; each one's share is 40.00. The sample input is 03_classes/trip.txt.
//
//   No new language features: this is exercises 1-7 in one program. Classes
//   with invariants (1), validating parsing (2), rule of zero (5), Money
//   with operators (6), reading stdin line by line (01_basics/08).
//
// YOU WILL LEARN (by putting it together)
//   C++
//   - several classes that cooperate, each guarding its own invariant.
//   - separating parsing (text -> objects; can fail, reports errors) from
//     logic (objects -> result; can't fail).
//   - std::map basics: a sorted key -> value dictionary.
//   Data structures & algorithms
//   - splitting money exactly: 100.00 / 3 is 33.34 + 33.33 + 33.33, never
//     3 x 33.33 = 99.99. A grosz must not disappear.
//   - an invariant over a whole collection: all balances always sum to 0.
//   - a greedy algorithm: at each step take the locally best move (largest
//     debtor pays largest creditor). Simple, fast, and good enough here,
//     though not always the minimum number of transfers.
//   - (bonus) a priority queue (binary heap) to get the largest element in
//     O(log n) instead of scanning in O(n).
//
// STEPS
// 1. Money. Copy Money and its std::formatter from exercise 6 (each exercise
//    is one file, so copying is expected). PLN only is fine: you may drop
//    Currency and the mismatch checks. You need: the explicit (cents)
//    constructor, Money() = default, cents(), to_string(), ==, <=>, +, -,
//    +=, -=, unary -.
//
// 2. Parsing amounts. Add
//      static Money parse(std::string_view text);   // throws std::invalid_argument
//    The message must be std::format("invalid amount \"{}\"", text): main
//    prints it as-is. Accepted: whole number, or a whole number, '.', and one
//    or two digits. No sign, no thousands separator, no double (exercise 6).
//    Split on '.', check the characters are digits, convert with
//    std::from_chars (<charconv>; 01_basics/05 step 5) or by hand.
//    "12.5" means 12.50, not 12.05.
//      assert(Money::parse("12") == Money{1200});
//      assert(Money::parse("12.5") == Money{1250});
//      assert(Money::parse("12.50") == Money{1250});
//      assert(Money::parse("0.99") == Money{99});
//      for (std::string_view bad :
//           {"", "12.", ".5", "12.555", "-3", "1,50", "abc", "12a", "1.2.3"}) {
//          assert(throws_invalid([bad] { (void)Money::parse(bad); }));
//      }
//    ([bad] lets the lambda use the loop variable.)
//
// 3. Exact split.
//      std::vector<Money> split(int parts) const;   // assert(parts > 0)
//    The parts differ by at most one grosz, the bigger ones come FIRST, and
//    they ALWAYS add up to the original. base = cents / parts, and the first
//    (cents % parts) parts get one grosz more. Amounts are positive here.
//      assert((Money{10000}.split(3) == std::vector{Money{3334}, Money{3333}, Money{3333}}));
//      assert((Money{100}.split(4) == std::vector{Money{25}, Money{25}, Money{25}, Money{25}}));
//      assert((Money{2}.split(3) == std::vector{Money{1}, Money{1}, Money{0}}));
//    (The extra parentheses keep assert, a macro, from splitting the
//    argument at the commas. vector == vector compares element by element.)
//    -> A3: Splitwise, Tricount and every bank split bills this way. What
//       happens over a year of group expenses if you round each share and
//       ignore the remainder?
//
// 4. The ledger.
//      class Ledger {
//        public:
//          void add_expense(const std::string& payer, Money amount,
//                           const std::vector<std::string>& participants);
//          Money balance_of(const std::string& person) const;   // > 0: the group owes them
//          const std::map<std::string, Money>& balances() const;
//          bool empty() const;
//        private:
//          bool invariant_holds() const;            // the sum of all balances is 0
//          std::map<std::string, Money> balances_;  // <map>, sorted by name
//      };
//    add_expense: throw std::invalid_argument for amount <= Money{} or no
//    participants. The payer's balance goes UP by amount; participant i goes
//    DOWN by split()[i] (so the extra grosze go to whoever is listed first).
//    End with assert(invariant_holds()).
//    std::map in 5 lines (the full story is 06_stl):
//      balances_[name] += x;      // [] creates the entry with Money{} if it's missing:
//                                 // that's why Money needs a default constructor
//      balances_.contains(name)   // true/false
//      balances_.at(name)         // the value; throws std::out_of_range if missing
//      for (const auto& [name, balance] : balances_) { ... }   // in name order
//    balance_of returns Money{} for an unknown person. It is const, so it
//    can't use [] — read the error if you try.
//    -> A4: why can't operator[] be used on a const map?
//    Test with the SCENARIO example:
//      Ledger l;
//      l.add_expense("ala", Money{12000}, {"ala", "bartek", "celina"});
//      assert(l.balance_of("ala") == Money{8000});
//      assert(l.balance_of("bartek") == Money{-4000});
//      assert(l.balance_of("darek") == Money{});
//
// 5. Settling up: a greedy algorithm.
//      struct Transfer { std::string from; std::string to; Money amount; };
//      std::vector<Transfer> settle(const Ledger& ledger);
//    Work on a COPY of the balances:
//      repeat:
//        debtor   = the most negative balance (if none is negative: stop)
//        creditor = the most positive balance
//        amount   = min(-debtor's balance, creditor's balance)    (std::min works: you have <=>)
//        record Transfer{debtor, creditor, amount}; debtor += amount; creditor -= amount
//    Ties: take the alphabetically first name. std::map iterates in name
//    order, so a strict comparison (< or >, not <= or >=) while scanning
//    gives that for free. Every step brings at least one person to zero,
//    so it ends after at most n - 1 transfers.
//    Check: apply the transfers to a copy of the balances; all must be 0 (assert).
//    -> A5: run the greedy algorithm BY HAND on five people:
//       a +4.00, b +3.00, c -3.00, d -2.00, e -2.00. How many transfers?
//       Now find a solution with one transfer fewer. (Hint: look for groups
//       whose balances add up to zero on their own.) Finding the true minimum
//       is NP-hard in general, which is why real apps settle for greedy.
//
// 6. Reading the input.
//      Ledger read_expenses(std::istream& in);
//    std::getline line by line, count line numbers from 1, split into words
//    with std::istringstream (01_basics/08). Skip empty lines and lines whose
//    first word starts with '#'. Fewer than 3 words is an error:
//      line 10: expected "payer amount participant..."
//    A bad amount: the message from Money::parse:
//      line 7: invalid amount "12.555"
//    Errors go to stderr, `std::println(stderr, "line {}: {}", n, e.what());`,
//    and the line is skipped; reading continues. Test it before using stdin:
//      std::istringstream input{"# comment\n\nala 30 bob\nbob x ala\nbob 10\n"};
//      Ledger t = read_expenses(input);       // prints two errors (lines 4 and 5)
//      assert(t.balance_of("ala") == Money{3000});
//
// 7. main. The tests print things ("ok, threw", the step 6 errors), and the
//    real output must stay clean, so the tests run only on request, like
//    the self-test flag many command-line tools have. Move all asserts into
//    `void self_test()` and write
//      int main(int argc, char* argv[])
//    If argv[1] is "--test" (compare via std::string_view{argv[1]}, and
//    check argc > 1 first): self_test(), print "all tests passed", return 0.
//    Otherwise read std::cin. Empty ledger: print "no expenses" and
//    return 1. Otherwise:
//      std::println("Balances:");
//      for each person:  std::println("  {:<8}{:>14}", name, with_sign(balance));
//      std::println("Transfers:");
//      for each transfer: std::println("  {} -> {}: {}", from, to, amount);
//    with `std::string with_sign(Money m)` adding '+' in front of positive
//    amounts.
//
// TEST RUNS (run from the cpp/ folder)
//   make 08_split_bill
//
//   ./bin/08_split_bill --test
//      ... the "ok, threw" lines and the two errors from step 6 ...
//      all tests passed
//
//   ./bin/08_split_bill < 03_classes/trip.txt
//      line 7: invalid amount "12.555"
//      line 10: expected "payer amount participant..."
//      Balances:
//        ala        +205.16 PLN
//        bartek     -142.58 PLN
//        celina     -157.07 PLN
//        darek       +94.49 PLN
//      Transfers:
//        celina -> ala: 157.07 PLN
//        bartek -> darek: 94.49 PLN
//        bartek -> ala: 48.09 PLN
//
//   printf 'ala 100 ala bartek celina\nbartek 50 ala\n' | ./bin/08_split_bill
//      Balances:
//        ala         +16.66 PLN
//        bartek      +16.67 PLN
//        celina      -33.33 PLN
//      Transfers:
//        celina -> bartek: 16.67 PLN
//        celina -> ala: 16.66 PLN
//
//   ./bin/08_split_bill < /dev/null; echo "exit code $?"
//      no expenses
//      exit code 1
//
//   Check trip.txt by hand for one person first (Ala: +360.00 paid, -90.00,
//   -33.34, -61.50 shares, +30.00 paid for Celina). Who gets the extra grosz
//   in the 245.99 split, and why?
//
// 8. Bonus: a priority queue. settle() scans every balance at each step:
//    O(n) per transfer, O(n^2) total. For a company of 10 000 employees
//    that's 10^8 operations. Rewrite it with two std::priority_queue
//    (<queue>; a binary heap, top() is the largest, push/pop are O(log n)):
//    one for creditors keyed by amount, one for debtors keyed by -amount;
//    pop both, transfer, push back whichever isn't at zero. O(n log n).
//    Store std::pair<Money, std::string> so equal amounts compare by name.
//    Careful: the heap pops the LARGEST pair, so ties now go to the
//    alphabetically LAST name; the transfers may come out in a different
//    order. Assert that both versions settle every balance to zero and use
//    the same number of transfers on trip.txt.
//
// DONE WHEN
//   [ ] the trip.txt, printf and /dev/null runs print exactly the output above
//   [ ] --test passes every assert from steps 2-6, zero warnings, ASan quiet
//   [ ] A3-A5 answered
//   [ ] you can explain: why split() puts the remainder in the first parts,
//       why the balances always sum to zero, and what "greedy" means
//
// Concepts: https://en.cppreference.com/w/cpp/container/map
//           https://en.cppreference.com/w/cpp/utility/from_chars
//           https://en.cppreference.com/w/cpp/container/priority_queue
//           https://en.wikipedia.org/wiki/Greedy_algorithm

#include <algorithm>
#include <cassert>
#include <charconv>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <map>
#include <print>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Test helper (see 02_time_of_day.cpp): true if f() throws std::invalid_argument.
template <typename F> bool throws_invalid(F f) {
    try {
        f();
    } catch (const std::invalid_argument& e) {
        std::println("  ok, threw: {}", e.what());
        return true;
    }
    return false;
}

int main() {
    // TODO
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A3:
// A4:
// A5:
