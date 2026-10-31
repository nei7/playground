// Exercise 2 — constructors: time of day for a booking system.
//
// SCENARIO
//   A booking app for a small hair salon. Appointments start at a time of
//   day like 09:30. Times come from code (`TimeOfDay(9, 30)`), from config
//   and user input ("09:30"), and from arithmetic ("start + 45 minutes").
//   A TimeOfDay must never hold 24:00, 9:75 or -1:00, so every way of
//   creating one must check its input. Constructors are those ways. At the
//   end you use the class for real work: finding double bookings and free
//   slots in a day's schedule.
//
// YOU WILL LEARN
//   C++
//   - the constructor is the only way to create an object: validate there,
//     throw if the input is bad, and an invalid object never exists.
//   - the member initializer list `: a_{...}, b_{...}` INITIALIZES; an
//     assignment in the body assigns to a member that was already
//     initialized. const members and references can only be initialized.
//   - members are initialized in DECLARATION order, whatever order the
//     initializer list says (a classic bug; -Wreorder catches it).
//   - delegating constructors: one constructor calls another, so the
//     validation lives in one place.
//   - the default constructor disappears once you declare any constructor;
//     `= default` brings it back.
//   - `explicit` on one-argument constructors stops silent conversions.
//   - named constructors (static member functions) when a plain constructor
//     would be ambiguous.
//   - the representation is private, so you can change it without touching
//     the calling code.
//   Data structures & algorithms
//   - intervals as half-open [start, end): two bookings overlap iff
//     a.start < b.end && b.start < a.end; touching is not overlapping.
//   - sort + sweep: sorting by start (O(n log n)) lets one left-to-right pass
//     find conflicts and gaps, instead of comparing all pairs (O(n^2)).
//
// STEPS
// 1. The validating constructor. Write
//      class TimeOfDay {
//        public:
//          TimeOfDay(int hours, int minutes);   // throws std::invalid_argument
//          int hours() const;
//          int minutes() const;
//          std::string to_string() const;       // "09:05": std::format("{:02}:{:02}", ...)
//        private:
//          int hours_;
//          int minutes_;
//      };
//    Valid: 0 <= hours <= 23 and 0 <= minutes <= 59. The message shows the
//    bad input: std::format("invalid time {:02}:{:02}", hours, minutes).
//    Tests (throws_invalid is the helper above main):
//      assert(TimeOfDay(9, 5).to_string() == "09:05");
//      assert(TimeOfDay(0, 0).to_string() == "00:00");
//      assert(TimeOfDay(23, 59).to_string() == "23:59");
//      assert(throws_invalid([] { TimeOfDay t(24, 0); }));
//      assert(throws_invalid([] { TimeOfDay t(12, 60); }));
//      assert(throws_invalid([] { TimeOfDay t(-1, 30); }));
//
// 2. The default constructor. Write `TimeOfDay t;` in main and read the
//    error: once you declare any constructor, the compiler stops generating
//    the default one. Keep the line commented out with the message.
//    The app needs a default (e.g. `std::vector<TimeOfDay> slots(16);`
//    default-constructs 16 elements: try it now, note the error), and
//    midnight is a sensible one.
//    Change the fields to `int hours_ = 0;` and `int minutes_ = 0;` and add
//    `TimeOfDay() = default;`. Assert TimeOfDay{}.to_string() == "00:00".
//    -> A2: without the `= 0`, what would `TimeOfDay t;` hold?
//       (01_basics/02, step 1.)
//
// 3. Delegating constructor: parsing text. Add
//      explicit TimeOfDay(std::string_view hhmm);    // "09:30"
//    It checks the FORMAT and leaves the RANGE check to the (int, int)
//    constructor:
//      TimeOfDay::TimeOfDay(std::string_view hhmm)
//          : TimeOfDay(two_digits(hhmm, 0), two_digits(hhmm, 3)) {}
//    `two_digits` is a private static helper:
//      static int two_digits(std::string_view text, std::size_t pos);
//    It throws std::invalid_argument(std::format("invalid time format \"{}\"", text))
//    unless text has exactly 5 chars with ':' at index 2, and returns the
//    number made of text[pos] and text[pos + 1] (both must be '0'..'9';
//    the digit value is c - '0'). static = it needs no object: it runs
//    before the object exists.
//      assert(TimeOfDay{"09:30"}.to_string() == "09:30");
//      assert(throws_invalid([] { TimeOfDay{"9:30"}; }));
//      assert(throws_invalid([] { TimeOfDay{"09-30"}; }));
//      assert(throws_invalid([] { TimeOfDay{"ab:cd"}; }));
//      assert(throws_invalid([] { TimeOfDay{"24:00"}; }));   // format ok, range not
//    Run it and look at the messages: "24:00" is rejected by the range check
//    in the other constructor, the others by two_digits.
//    -> A3: why must the range check exist only once, in (int, int)? Describe
//       what goes wrong in six months if the string constructor kept its own
//       copy of `hours <= 23`.
//
// 4. explicit: a real bug. Add a NON-explicit constructor
//      TimeOfDay(int minutes_since_midnight);    // 570 -> 09:30, delegates to (int, int)
//    and this function above main:
//      void book(TimeOfDay start, int duration_minutes) {
//          std::println("booked {} for {} min", start.to_string(), duration_minutes);
//      }
//    The caller wants "09:30 for 45 minutes" but swaps the arguments:
//      book(45, 570);
//    Predict the output, then run. It compiles and books 00:45 for 570
//    minutes: 45 silently became a TimeOfDay. Mark the constructor explicit
//    and note the compile error `book(45, 570)` gives now.
//    `book(TimeOfDay{570}, 45)` compiles but still reads badly (570 what?).
//    Replace the constructor with a named one:
//      static TimeOfDay from_minutes(int minutes_since_midnight);
//    `book(TimeOfDay::from_minutes(570), 45)` can't be misread.
//    Rule: every one-argument constructor is explicit unless the conversion
//    is lossless and obvious (std::string from a string literal is one of the
//    rare good cases).
//
// 5. Arithmetic that keeps the invariant. Add
//      int minutes_since_midnight() const;
//      TimeOfDay plus_minutes(int minutes) const;   // wraps around midnight
//    It returns a NEW TimeOfDay (const member: the object itself never
//    changes, like std::string::substr). Watch out: in C++ -20 % 1440 is
//    -20, not 1420 (the remainder has the sign of the left operand). Use
//    ((m % 1440) + 1440) % 1440.
//      assert(TimeOfDay(9, 0).plus_minutes(90).to_string() == "10:30");
//      assert(TimeOfDay(23, 30).plus_minutes(45).to_string() == "00:15");
//      assert(TimeOfDay(0, 10).plus_minutes(-20).to_string() == "23:50");
//      assert(TimeOfDay(12, 0).plus_minutes(3 * 1440).to_string() == "12:00");
//
// 6. Change the representation. Replace the two private fields with one:
//      int minutes_ = 0;   // minutes since midnight, 0..1439
//    and rewrite the member functions (hours() is minutes_ / 60, ...).
//    Do NOT touch main. Run: every assert from steps 1-5 still passes. With
//    the public-field struct of 01_basics you'd now be fixing every `t.hours`
//    in the program. That's the payoff of private data.
//    -> A6: which member functions got simpler, which got more complicated?
//
// 7. Initialization order: the bug. Paste this class above main as-is:
//      class OpeningHours {
//        public:
//          OpeningHours(TimeOfDay open, int slot_minutes, int slot_count)
//              : open_{open}, slot_minutes_{slot_minutes},
//                close_{open_.plus_minutes(slot_minutes_ * slot_count)} {}
//          std::string to_string() const {
//              return open_.to_string() + "-" + close_.to_string();
//          }
//        private:
//          TimeOfDay open_;
//          TimeOfDay close_;
//          int slot_minutes_;
//      };
//    Build and read BOTH warnings ([-Wreorder] and [-Wuninitialized]).
//    What SHOULD `OpeningHours{TimeOfDay(9, 0), 30, 16}.to_string()` print?
//    (16 slots of 30 minutes from 09:00.) Print it and compare. You get
//    whatever slot_minutes_ held in memory: often 09:00-09:00, sometimes
//    garbage. Fix it by reordering the DECLARATIONS (not the list).
//    -> A7: why didn't the order in the initializer list matter?
//
// 8. Members that can't be assigned. Give OpeningHours a
//    `const std::string salon_name_;` and a constructor parameter for it.
//    First set it "Java-style" in the constructor body: `salon_name_ = name;`.
//    Note the error, then move it into the initializer list.
//    Side effect: with a const member, `a = b;` no longer compiles for the
//    whole class (try it with two OpeningHours, note the error). That's
//    why const data members are rare: private + no setter protects the
//    field just as well and keeps the class assignable. Remove the const.
//    -> A8: name the three kinds of members that MUST be initialized in the
//       list (one is const, one you haven't used: a reference, and one is a
//       member whose type has no default constructor).
//
// 9. Real work: a day's bookings. Add above main:
//      struct Booking {
//          std::string client;
//          TimeOfDay start;
//          int minutes;
//          TimeOfDay end() const;   // start + minutes; assume no booking crosses midnight
//      };
//    To sort by time, TimeOfDay needs comparisons. Add inside the class:
//      auto operator<=>(const TimeOfDay&) const = default;
//    (one line gives <, <=, >, >=, == and !=; exercise 6 explains it. It's
//    correct here because the only member is minutes_.) The day:
//      std::vector<Booking> day{
//          {"Kasia", TimeOfDay(9, 0), 60},  {"Ola", TimeOfDay(10, 30), 45},
//          {"Basia", TimeOfDay(9, 45), 30}, {"Zosia", TimeOfDay(13, 0), 90},
//          {"Ewa", TimeOfDay(11, 15), 30},  {"Marta", TimeOfDay(14, 0), 60},
//      };
//    a) Brute force: `int count_conflicts_naive(const std::vector<Booking>&)`
//       compares every pair i < j. Two bookings overlap iff
//       a.start < b.end() && b.start < a.end(). Assert it returns 2.
//       Ola (10:30-11:15) and Ewa (11:15-11:45) touch but don't overlap: half-open
//       intervals [start, end) make "ends when the next one starts" legal.
//    b) Sort + sweep: `void print_conflicts(std::vector<Booking> day)` (by value:
//       it sorts its own copy) sorts with
//         std::ranges::sort(day, {}, &Booking::start);    // sort by the start member
//       then walks left to right remembering the booking that ends latest so
//       far. A booking that starts before that end is a conflict. Output:
//         conflict: Basia 09:45-10:15 overlaps Kasia 09:00-10:00
//         conflict: Marta 14:00-15:00 overlaps Zosia 13:00-14:30
//    c) Free slots: `void print_free(std::vector<Booking> day, TimeOfDay open, TimeOfDay close)`.
//       Same sort, then a cursor starting at `open`: a booking that starts
//       after the cursor leaves a gap; then move the cursor to
//       max(cursor, booking end). For 09:00-17:00 the output is:
//         free: 10:15-10:30
//         free: 11:45-13:00
//         free: 15:00-17:00
//       (This is the "merge intervals" algorithm; every calendar app runs it.)
//    -> A9: the naive version compares n*(n-1)/2 pairs. For 10 000 bookings
//       (a city-wide system), roughly how many comparisons is that, and how
//       many does sort + sweep need (n log2 n + n)?
//
// DONE WHEN
//   [ ] all asserts pass, zero warnings (after the fix in step 7)
//   [ ] steps 6 and 9 needed no change in the tests from steps 1-5
//   [ ] the step 9 output matches exactly
//   [ ] A2-A9 answered
//   [ ] you can explain: why validate in the constructor, why explicit, and
//       in which order members are initialized
//
// Concepts: https://en.cppreference.com/w/cpp/language/constructor
//           https://en.cppreference.com/w/cpp/language/explicit
//           https://en.cppreference.com/w/cpp/language/default_constructor
//           https://en.cppreference.com/w/cpp/algorithm/ranges/sort
//           https://en.wikipedia.org/wiki/Interval_scheduling

#include <algorithm>
#include <cassert>
#include <compare>
#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Test helper: true if calling f() throws std::invalid_argument.
// Use it like this:   assert(throws_invalid([] { TimeOfDay t(24, 0); }));
// `[] { ... }` is a lambda: a piece of code passed to a function to be run
// there. Lambdas and templates are 05_templates; until then treat this line
// as fixed syntax for "run this code and tell me whether it threw".
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
// A2:
// A3:
// A4 (prediction for book(45, 570), before running):
// A6:
// A7:
// A8:
// A9:
