// Exercise 1 — encapsulation and invariants: a bank account.
//
// SCENARIO
//   The core of a banking app. An account has an owner and a balance; money
//   comes in and goes out. One rule must never break: the balance is never
//   negative. Below main's TODO is the version most people write first (a
//   struct plus a function). You will break it in one line, then turn it into
//   a class that no outside code can put into a bad state. At the end you add
//   a bank statement and answer an auditor's questions about it quickly,
//   using prefix sums.
//
// YOU WILL LEARN
//   C++
//   - invariant: a condition that holds for every object of the class
//     whenever none of its member functions is running ("balance >= 0").
//   - the constructor establishes the invariant; every public member function
//     must preserve it. Private data makes "every" checkable: only the
//     class's own functions can touch it.
//   - class vs struct: the only language difference is default access
//     (class = private, struct = public). The convention: struct when the
//     fields can vary independently, class when there is an invariant.
//   - interface made of operations (deposit, withdraw), not of fields
//     (set_balance).
//   - a getter returning const&: callers read without copying and without
//     getting write access.
//   - assert(invariant_holds()) at the end of every mutating member: a cheap
//     debugging technique that stops the program at the first broken rule.
//   Data structures & algorithms
//   - prefix sums: precompute running totals once (O(n)), then answer "sum
//     of transactions k..m" in O(1) instead of O(n) per question.
//   - half-open ranges [first, last): the convention behind every iterator
//     pair in the standard library.
//
// Money is stored in cents/grosze as long long: 12.50 PLN is 1250, written
// 12'50 (the ' is a digit separator; the compiler ignores it). Why never
// double for money is exercise 6.
//
// STEPS
// 1. Break the struct version. In main:
//      AccountV1 acc{"Ala", 100'00};
//      assert(withdraw_v1(acc, 30'00));
//      assert(acc.balance == 70'00);
//      assert(!withdraw_v1(acc, 500'00));     // the check works...
//      acc.balance -= 500'00;                 // ...until someone writes this
//      std::println("balance: {}", acc.balance);
//    It compiles and prints -43000. The rule lives in a comment; nothing
//    enforces it. Keep these lines (they are the "before" picture).
//    -> A1: name two OTHER ways outside code can break the rule with AccountV1.
//
// 2. Write the class below the "version 2" marker:
//      class Account {
//        public:
//          Account(std::string owner, long long initial_cents);
//          const std::string& owner() const;
//          long long balance() const;
//          void deposit(long long cents);
//          bool withdraw(long long cents);   // false and no change if not enough money
//        private:
//          std::string owner_;
//          long long balance_ = 0;
//      };
//    Define the member functions BELOW the class (`long long Account::balance()
//    const { return balance_; }`), not inside it — that's how real headers
//    and .cpp files are split.
//    Constructor: `: owner_{std::move(owner)}, balance_{initial_cents}`.
//    Taking the string by value and moving it into the member is the standard
//    way to store a string parameter (why it's cheap: 04_move).
//    Then write `acc2.balance_ -= 500'00;` for an Account, read the error,
//    keep the line commented out with the message after it.
//
// 3. Guard the invariant. Write it as a comment directly above `class Account`:
//      // Invariant: balance_ >= 0.
//    Then:
//      - constructor: initial_cents < 0 -> throw std::invalid_argument with
//        std::format("initial balance must be >= 0, got {}", initial_cents)
//      - deposit:  cents <= 0 -> throw std::invalid_argument
//      - withdraw: cents <= 0 -> throw std::invalid_argument;
//                  cents > balance_ -> return false, nothing changes
//    Test every branch. For a throw, use this pattern (no test framework yet):
//      bool threw = false;
//      try {
//          Account bad{"X", -5};
//      } catch (const std::invalid_argument& e) {
//          threw = true;
//          std::println("expected: {}", e.what());
//      }
//      assert(threw);
//    -> A3: withdraw() with too little money returns false, but deposit(-5)
//       throws. Why the difference? (Which one is a normal event at a bank,
//       and which one can only be a bug in the calling code?)
//
// 4. The setter trap. Add `void set_balance(long long cents) { balance_ = cents; }`
//    to the public part and break the invariant with it in one line of main.
//    The field is private, yet the setter made it public again. Delete the
//    setter, comment the line out with a note.
//    Rule: offer operations that exist in the domain (a bank has deposits and
//    withdrawals, not "set balance").
//
// 5. History. Above the class add:
//      struct Transaction {
//          enum class Kind { Deposit, Withdrawal };
//          Kind kind;
//          long long cents;    // always > 0; the sign comes from kind
//      };
//    Add a private `std::vector<Transaction> history_;`, record every
//    successful deposit and withdrawal (and the initial balance as a deposit
//    if it is > 0), and expose it:
//      const std::vector<Transaction>& history() const;
//    Second invariant: balance_ == sum of deposits - sum of withdrawals.
//    Write a private `bool invariant_holds() const` checking BOTH invariants
//    and put `assert(invariant_holds());` as the last line of the
//    constructor, deposit and withdraw.
//    Experiment: make the getter return `std::vector<Transaction>&` (drop
//    `const` in both places), call `acc.history().clear();` in main, then
//    `acc.deposit(1)`. Which assert fires, and in which function? Revert to
//    const& and note the compile error the clear() line now gives.
//    -> A5: why return const& rather than a copy `std::vector<Transaction>`?
//       (Both are safe. Think of an account with 50 000 transactions and a
//       loop that calls history().size().)
//
// 6. A function outside the class:
//      bool transfer(Account& from, Account& to, long long cents);
//    All or nothing: if `from` can't pay, neither account changes and it
//    returns false. It uses only the public interface. Test:
//      Account ala{"Ala", 100'00}, ola{"Ola", 0};
//      assert(transfer(ala, ola, 30'00));      // 70'00 / 30'00
//      assert(!transfer(ala, ola, 1000'00));   // both unchanged
//    -> A6: transfer can't see balance_. Why is that an advantage? (What
//       would you have to change in transfer if balance_ were stored
//       differently, e.g. as a std::int64_t or as euro + cents?)
//
// 7. The statement (const correctness + running balance). Write
//      std::string format_cents(long long cents);   // 37926 -> "379.26"; cents >= 0
//      void print_statement(const Account& acc);
//    Use std::format("{}.{:02}", cents / 100, cents % 100). Each line shows
//    the transaction and the balance after it:
//      std::println("  {:>10} {:>10}", sign + format_cents(t.cents), format_cents(running));
//    (sign is '+' or '-'; char + std::string works.) For this scenario:
//      Account ala{"Ala", 0};
//      ala.deposit(2500'00);                // salary
//      ala.withdraw(1800'00);               // rent
//      ala.withdraw(350'75);                // groceries
//      assert(!ala.withdraw(600'00));       // declined: not in the history
//      ala.deposit(120'00);                 // refund
//      ala.withdraw(89'99);                 // internet
//    the output must be exactly:
//      Statement for Ala
//          +2500.00    2500.00
//          -1800.00     700.00
//           -350.75     349.25
//           +120.00     469.25
//            -89.99     379.26
//      Balance: 379.26
//    print_statement takes const Account&. Temporarily remove `const` from
//    balance() (declaration and definition): read the error at the call in
//    print_statement. Put it back.
//    Why format_cents only accepts cents >= 0 here: the invariant guarantees
//    every running balance is >= 0. (Negative amounts are exercise 6.)
//
// 8. Prefix sums: the auditor's questions. An auditor asks thousands of
//    times: "how much money moved between transaction i and transaction j?"
//    Summing the range each time is O(j - i) per question. Instead,
//    compute running totals once:
//      std::vector<long long> prefix_sums(const std::vector<Transaction>& h);
//        // size h.size() + 1; result[0] = 0; result[k] = signed sum of h[0..k)
//      long long flow(const std::vector<long long>& prefix, std::size_t first,
//                     std::size_t last);
//        // signed sum of transactions with index in [first, last):
//        // prefix[last] - prefix[first]
//    For the history from step 7:
//      auto p = prefix_sums(ala.history());
//      assert((p == std::vector<long long>{0, 250000, 70000, 34925, 46925, 37926}));
//      assert(flow(p, 1, 4) == -203075);   // rent + groceries + refund
//      assert(flow(p, 0, 5) == ala.balance());
//      assert(flow(p, 2, 2) == 0);         // empty range
//    Then write `long long lowest_balance(const std::vector<long long>& prefix)`
//    (the minimum over prefix[1..]) and assert it is 34925 here (after the
//    groceries). This is exactly what the bank computes for "lowest balance
//    this month" on a statement.
//    -> A8: with [first, last), how many elements does the range have, and
//       how do you write an empty range? Why is that nicer than [first, last]?
//
// 9. struct or class? -> A9: for each, say struct (no invariant) or class
//    and write the invariant: Point{x, y}; Fraction{num, den}; Rgb{r, g, b}
//    stored as three std::uint8_t; EmailAddress; Card{suit, rank} from
//    01_basics/07; a Stopwatch that is either running or stopped.
//
// DONE WHEN
//   [ ] main runs to the end, no failed assert, zero warnings
//   [ ] the statement in step 7 matches character for character
//   [ ] the commented-out lines from steps 2, 4, 5 carry their error/effect
//   [ ] A1–A9 answered in the ANSWERS block at the bottom
//   [ ] you can say in one sentence: what an invariant is, who must keep it,
//       and why private data is what makes that possible
//
// Concepts: https://en.cppreference.com/w/cpp/language/access
//           https://en.cppreference.com/w/cpp/language/classes
//           https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c2-use-class-if-the-class-has-an-invariant-use-struct-if-the-data-members-can-vary-independently
//           https://en.wikipedia.org/wiki/Prefix_sum

#include <cassert>
#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// --- version 1: the struct everyone writes first -----------------------------
// Rule (only written in this comment): balance is never negative.
struct AccountV1 {
    std::string owner;
    long long balance = 0; // cents
};

bool withdraw_v1(AccountV1& acc, long long cents) {
    if (cents <= 0 || cents > acc.balance) {
        return false;
    }
    acc.balance -= cents;
    return true;
}

// --- version 2: class Account (steps 2-5) ------------------------------------
struct Transaction {
    enum class Kind { Deposit, Withdrawal };
    Kind kind;
    long long cents;
};

class Account {
  public:
    Account(std::string owner, long long initial_cents);
    const std::string& owner() const;
    long long balance() const;
    void deposit(long long cents);
    bool withdraw(long long cents);
    std::vector<Transaction>& history();
    // void set_balance(long long cents) {
    //     balance_ = cents;
    // }

  private:
    std::vector<Transaction> history_;
    std::string owner_;
    long long balance_ = 0;
    bool invariant_holds() const;
};

bool Account::invariant_holds() const {
    long long total = 0;
    for (auto transaction : history_) {
        switch (transaction.kind) {
        case Transaction::Kind::Deposit:
            total += transaction.cents;
            break;
        case Transaction::Kind::Withdrawal:
            total -= transaction.cents;
            break;

        default:
            std::unreachable();
        }
    }

    return total >= 0;
}

std::vector<Transaction>& Account::history() {
    return history_;
}

Account::Account(std::string owner, long long initial_cents)
    : owner_{std::move(owner)}, balance_{initial_cents} {
    if (initial_cents < 0)
        throw std::invalid_argument(
            std::format("initial balance must be >= 0, got {}", initial_cents));

    history_.push_back(Transaction(Transaction::Kind::Deposit, initial_cents));

    assert(invariant_holds());
}

const std::string& Account::owner() const {
    return owner_;
}

long long Account::balance() const {
    return balance_;
}

void Account::deposit(long long cents) {
    if (cents <= 0)
        throw std::invalid_argument("deposited value can't be zero or negative");

    balance_ += cents;

    history_.push_back(Transaction(Transaction::Kind::Deposit, cents));
    assert(invariant_holds());
}

bool Account::withdraw(long long cents) {
    if (cents <= 0 || cents > balance_) {
        return false;
    }
    balance_ -= cents;

    history_.push_back(Transaction(Transaction::Kind::Withdrawal, cents));
    assert(invariant_holds());

    return true;
}

int main() {
    // TODO
    AccountV1 acc{"Ala", 100'00};
    assert(withdraw_v1(acc, 30'00));
    assert(acc.balance == 70'00);
    assert(!withdraw_v1(acc, 500'00)); // the check works...
    acc.balance -= 500'00;             // ...until someone writes this
    std::println("balance: {}", acc.balance);

    bool threw = false;
    try {
        Account bad{"X", -5};
    } catch (const std::invalid_argument& e) {
        threw = true;
        std::println("expected: {}", e.what());
    }
    assert(threw);

    Account acc2("ala2", 3000);
    acc2.history().clear();
    acc2.deposit(1);
    // acc2.set_balance(-1000);
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A1: it can also change its owner with the exact mechanism
// A3: obviously  depositing a negative value is abnormal situation that can only occur by calling
// the code
// A5:  because copying is too expensive for that big structures
//
//  A6: A8: A9:
//   Point:
//   Fraction:
//   Rgb:
//   EmailAddress:
//   Card:
//   Stopwatch:
