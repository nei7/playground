// Exercise 6 — operator overloading: money for an online shop.
//
// SCENARIO
//   An online shop: a product catalog, a cart, VAT, a printed invoice.
//   Money is the classic case for a custom value type. double gives wrong
//   cents, and adding euros to złoty must be an error, not a silent bug.
//   With operators, code using Money reads like arithmetic:
//   `net + vat`, `price * 3`, `if (total > budget)`, std::println("{}", total).
//   With comparisons in place you also get sorting and binary search on
//   prices for free, and you write the binary search yourself once.
//
// YOU WILL LEARN
//   C++
//   - why money is never double.
//   - the canonical pattern: compound assignment (+=) as a member that
//     returns *this, the binary operator (+) as a non-member built on it.
//   - why some operators must be non-members: `3 * price`.
//   - operator== and operator<=>: `= default` vs hand-written, and how to
//     tell when the default is WRONG.
//   - printing your type: operator<< for streams, std::formatter for
//     std::format / std::println (with width and alignment working).
//   - only overload operators that mean something in the domain: no
//     Money * Money; and when an operation needs a policy (rounding), give
//     it a name instead of an operator.
//   - hidden friends: operators defined as friends inside the class.
//   Data structures & algorithms
//   - sorting with a projection (sort products by their price member).
//   - binary search on sorted data: O(log n) instead of O(n); writing it
//     by hand without off-by-one errors, half-open ranges again.
//
// STEPS
// 1. Why not double. Predict each line, then run:
//      double total = 0.0;
//      for (int i = 0; i < 10; ++i) {
//          total += 0.10;
//      }
//      std::println("{} {}", total, total == 1.0);
//      std::println("{:.20f}", 0.10);
//      std::println("{}", static_cast<long long>(19.99 * 100));
//      std::println("{}", static_cast<long long>(0.29 * 100));
//    The last two are a real bug: converting a price to grosze by
//    multiplying and truncating loses one grosz (19.99 becomes 1998).
//    0.10 has no exact binary representation, just like 1/3 has no exact
//    decimal one.
//    -> A1: what did each line print, and what is the fix in one sentence?
//
// 2. The type.
//      enum class Currency { PLN, EUR };
//      std::string_view to_string(Currency c);         // "PLN", "EUR"
//      class Money {
//        public:
//          Money() = default;                           // 0.00 PLN
//          explicit Money(std::int64_t cents, Currency currency = Currency::PLN);
//          std::int64_t cents() const;
//          Currency currency() const;
//          std::string to_string() const;               // "19.99 PLN"
//        private:
//          std::int64_t cents_ = 0;
//          Currency currency_ = Currency::PLN;
//      };
//    Why explicit: thanks to the default argument this is ALSO a one-argument
//    constructor. Without explicit, `Money m = 5;` compiles and means five
//    grosze. Try it both ways, note the error.
//    to_string, the negative-number trap: the naive
//    std::format("{}.{:02} {}", cents_ / 100, cents_ % 100, ...) prints -50
//    grosze as "0.-50 PLN" (-50 / 100 is 0, -50 % 100 is -50). Write the
//    naive one, see it fail, then fix it: print the sign separately and
//    format the absolute value.
//      assert(Money{1999}.to_string() == "19.99 PLN");
//      assert(Money{5}.to_string() == "0.05 PLN");
//      assert(Money{-50}.to_string() == "-0.50 PLN");
//      assert(Money{-12345, Currency::EUR}.to_string() == "-123.45 EUR");
//      assert(Money{}.to_string() == "0.00 PLN");
//
// 3. Equality. `assert(Money{100} == Money{100});` doesn't compile yet.
//    Add `bool operator==(const Money&) const = default;` (member by member:
//    same cents AND same currency). 100 PLN != 100 EUR: correct.
//
// 4. Arithmetic, the canonical way.
//      Money& operator+=(const Money& other);   // member: changes *this, returns *this
//      Money& operator-=(const Money& other);
//      Money operator-() const;                 // unary minus: -price
//    and as hidden friends inside the class:
//      friend Money operator+(Money a, const Money& b) { a += b; return a; }
//      friend Money operator-(Money a, const Money& b) { a -= b; return a; }
//    `a` is taken by value: it's already the copy we return. += and -= throw
//    std::invalid_argument("currency mismatch: PLN vs EUR") for different
//    currencies (write one private helper `void check_same_currency(const Money&) const`).
//      assert(Money{1999} + Money{1} == Money{2000});
//      assert(Money{500} - Money{800} == Money{-300});
//      assert(-Money{250} == Money{-250});
//      Money m{100};  m += Money{50};  assert(m == Money{150});
//      assert(throws_invalid([] { (void)(Money{100} + Money{100, Currency::EUR}); }));
//    (`(void)(...)` means "I ignore this result on purpose"; it keeps
//    the build warning-free.)
//    Why += returns Money&: so `a += b += c` works and nothing is copied,
//    the same contract as for int.
//
// 5. Multiplication by a quantity: two directions.
//      friend Money operator*(Money price, std::int64_t quantity);
//      friend Money operator*(std::int64_t quantity, Money price);   // calls the other one
//    Try writing the second one as a MEMBER first: impossible, because a member
//    operator always has the class object on the LEFT. That's the rule:
//    binary operators where the left side may be something else are non-members.
//      assert(Money{2499} * 3 == Money{7497});
//      assert(3 * Money{2499} == Money{7497});
//    Then VAT. A percentage needs rounding, so it gets a NAME, not an operator:
//      Money percent(std::int64_t p) const;   // p% of the amount, rounded half away from zero
//    23% of 19.99 is 4.5977, i.e. 4.60. Integer division truncates:
//    1999 * 23 / 100 == 459, one grosz short. Round instead:
//    q = cents_ * p; result = (q >= 0 ? q + 50 : q - 50) / 100.
//      assert(Money{1999}.percent(23) == Money{460});
//      assert(Money{-1999}.percent(23) == Money{-460});
//      assert(Money{50}.percent(23) == Money{12});    // 11.5 -> 12
//      assert(Money{100}.percent(23) == Money{23});
//    -> A5: why is there no operator*(Money, Money)? And why is percent()
//       better than an operator*(Money, double)?
//
// 6. Ordering, and when `= default` lies. Add
//      auto operator<=>(const Money&) const = default;
//    Now <, <=, >, >= all compile. But defaulted <=> compares the members in
//    declaration order: cents first. So:
//      assert(Money{100, Currency::EUR} < Money{200});   // passes!
//    "1 euro is less than 2 złoty": false in reality (1 EUR is about 4.30 PLN).
//    Replace it with a hand-written version:
//      std::strong_ordering operator<=>(const Money& other) const;   // <compare>
//    which throws on a currency mismatch and otherwise returns
//    cents_ <=> other.cents_. (A hand-written <=> doesn't generate ==; your
//    defaulted == from step 3 stays.) Comment out the assert above (keep it
//    as the record of the bug) and add:
//      assert(throws_invalid([] { (void)(Money{100, Currency::EUR} < Money{200}); }));
//      assert(Money{100} < Money{200});
//      assert(Money{300} >= Money{300});
//      assert(Money{-1} < Money{});
//    -> A6: an alternative would be returning std::partial_ordering::unordered
//       for different currencies instead of throwing. What would `a < b` and
//       `a > b` then both return? Which is safer for a shop, and why?
//
// 7. Printing. Two ways, both used in real code:
//    a) streams: a hidden friend
//         friend std::ostream& operator<<(std::ostream& os, const Money& m) {
//             return os << m.to_string();
//         }
//       Test: std::cout << Money{1999} << '\n';
//    b) std::println: specialize std::formatter below the class:
//         template <>
//         struct std::formatter<Money> : std::formatter<std::string> {
//             auto format(const Money& m, std::format_context& ctx) const {
//                 return std::formatter<std::string>::format(m.to_string(), ctx);
//             }
//         };
//       Treat it as a recipe until 05_templates: "format Money by formatting
//       its to_string()". Because it reuses the string formatter, width and
//       alignment work:
//         std::println("[{:>12}] [{:<12}]", Money{1999}, Money{-50});
//       must print
//         [   19.99 PLN] [-0.50 PLN   ]
//
// 8. The invoice.
//      struct LineItem { std::string name; Money unit_price; int quantity; };
//      void print_invoice(const std::vector<LineItem>& items, int vat_percent);
//    VAT is computed ONCE on the net total (not per line: per-line rounding
//    can differ by a grosz; real invoicing systems pick one rule and stick
//    to it). Format strings:
//      header and lines:  "{:<16}{:>5}{:>14}{:>14}"   (name, qty, price, line total)
//      totals:            "{:>35}{:>14}"              (label, amount)
//    For
//      std::vector<LineItem> items{
//          {"Keyboard", Money{18999}, 1},
//          {"USB-C cable", Money{2499}, 3},
//          {"Mouse pad", Money{1550}, 2},
//      };
//    with 23% VAT the output must be exactly:
//      Item              Qty         Price         Total
//      Keyboard            1    189.99 PLN    189.99 PLN
//      USB-C cable         3     24.99 PLN     74.97 PLN
//      Mouse pad           2     15.50 PLN     31.00 PLN
//                                     Net:    295.96 PLN
//                                 VAT 23%:     68.07 PLN
//                                   Gross:    364.03 PLN
//    (quantity is int; `Money * int` needs a cast to std::int64_t, or an
//    extra overload. Decide which and why.)
//
// 9. The catalog: sort and binary search.
//      struct Product { std::string name; Money price; };
//      std::vector<Product> catalog{
//          {"HDMI cable", Money{2999}}, {"Laptop stand", Money{8900}}, {"Webcam", Money{19900}},
//          {"Mouse", Money{4999}},      {"Headset", Money{24900}},     {"Monitor", Money{89900}},
//          {"USB hub", Money{6450}},    {"Keyboard", Money{18999}},    {"Mouse pad", Money{1550}},
//          {"Desk lamp", Money{12900}},
//      };
//    a) Sort by price: std::ranges::sort(catalog, {}, &Product::price);
//       (this needs your <=>). Print the list; Mouse pad first, Monitor last.
//    b) The question "how many products can I afford with B?" on SORTED data
//       is a binary search for the first product with price > B. Write it
//       by hand:
//         std::size_t count_affordable(const std::vector<Product>& sorted, Money budget);
//       Keep a half-open window [lo, hi) = [0, size). Invariant: everything
//       before lo is <= budget, everything from hi on is > budget. While
//       lo < hi: mid = lo + (hi - lo) / 2; if sorted[mid].price <= budget then
//       lo = mid + 1, else hi = mid. Return lo.
//         assert(count_affordable(catalog, Money{10000}) == 5);
//         assert(count_affordable(catalog, Money{8900}) == 5);    // equal counts as affordable
//         assert(count_affordable(catalog, Money{1000}) == 0);
//         assert(count_affordable(catalog, Money{100000}) == 10);
//    c) The standard version is one line; compare both for budgets from 0 to
//       1000.00 PLN in steps of 0.50 (assert they agree on every budget):
//         auto it = std::ranges::upper_bound(catalog, budget, {}, &Product::price);
//         std::size_t n = static_cast<std::size_t>(it - catalog.begin());
//    -> A9: how many loop iterations does your binary search need for 10
//       products? For 1 000 000? Why `lo + (hi - lo) / 2` and not
//       `(lo + hi) / 2`?
//
// DONE WHEN
//   [ ] every assert passes, the invoice matches character for character
//   [ ] the defaulted <=> experiment from step 6 is kept as a comment
//   [ ] count_affordable agrees with std::ranges::upper_bound on all budgets
//   [ ] A1, A5, A6, A9 answered, zero warnings
//   [ ] you can explain: member vs non-member operators, why += returns a
//       reference, when `= default` comparison is wrong, and binary search's
//       loop invariant
//
// Concepts: https://en.cppreference.com/w/cpp/language/operators
//           https://en.cppreference.com/w/cpp/language/default_comparisons
//           https://en.cppreference.com/w/cpp/utility/format/formatter
//           https://en.cppreference.com/w/cpp/algorithm/ranges/upper_bound
//           https://en.wikipedia.org/wiki/Binary_search_algorithm

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
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
// A1:
// A5:
// A6:
// A9:
