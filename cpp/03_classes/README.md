# 03_classes — designing your own types (with data structures and algorithms)

Prerequisite: `02_memory/` (lifetime, destructors, unique_ptr, your RAII `File`).

So far you have used types (`std::string`, `std::vector`) and written structs
with public fields. This folder is about writing types that other code can't
misuse. The class guards its data, the constructor makes every object valid
from its first line, copies do the right thing, and operators make the type
read like a built-in one.

Every file is a small real program, and each one also teaches a data
structure or an algorithm the way it's used in practice. You don't get a
textbook definition. You get a problem where the naive solution is visibly
slow or wrong, and then you measure the fix.

## Files

Do them in order; each one uses the previous ones.

| File | C++ topic | Real-world piece | Data structure / algorithm |
|------|-----------|------------------|----------------------------|
| `01_bank_account.cpp` | encapsulation, invariants, class vs struct, const& getters | a bank account and its statement | prefix sums, half-open ranges |
| `02_time_of_day.cpp` | constructors: validation, delegating, default, explicit, named ctors, init order | a hair salon booking system | intervals, sort + sweep (conflicts, free slots) |
| `03_special_members.cpp` | the six special members, copy elision, noexcept moves, rule of five | finding hidden copies in a "slow editor" | dynamic array growth, amortized O(1) |
| `04_dynamic_array.cpp` | rule of three/five, deep copy, copy-and-swap | your own `std::vector` | growth strategies O(n) vs O(n²), insert/erase cost |
| `05_image.cpp` | rule of zero, unique_ptr with a custom deleter, explicit operator bool, type traits | an image editor (PGM files) and a robot vacuum | 2D grids, flood fill, BFS, shortest path, connected components |
| `06_money.cpp` | operator overloading, `==`/`<=>`, `operator<<`, `std::formatter` | prices, VAT and an invoice | sorting by key, binary search |
| `07_ring_buffer.cpp` | index invariants, composition | server latency monitoring, `tail` | ring buffer, sliding window sum, sliding window max (monotonic deque) |
| `08_split_bill.cpp` | capstone: everything above + `std::map` | splitting trip expenses (`trip.txt`) | exact money split, greedy settlement, priority queue |

## How each file is organised

- **SCENARIO** — the problem the code solves.
- **YOU WILL LEARN** — the concepts, C++ first, then data structures and
  algorithms. When the file is done you should be able to explain each one in
  a sentence or two without looking. That is the real "done" criterion.
- **STEPS** — numbered. Each one says what to write (exact signatures), what
  to run, and what you should see (exact asserts or exact output). Lines
  marked `-> A3:` are questions; answer them in the **ANSWERS** block at the
  bottom of the file.
- **DONE WHEN** — the checklist.

Signatures given in a step stay as they are. Where a step says "predict",
write the prediction in a comment *before* running.

## Measuring time

Several steps compare a naive algorithm with a better one. Run those with
`SAN=0` (the sanitizers slow every memory access and distort the
comparison), e.g. `make run-04_dynamic_array SAN=0`. The pattern:

```cpp
auto t0 = std::chrono::steady_clock::now();
// ... the work ...
auto t1 = std::chrono::steady_clock::now();
double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
```

Absolute numbers depend on the machine; what matters is how the time changes
when n doubles:

| Complexity | n → 2n makes the time | Typical example here |
|------------|-----------------------|----------------------|
| O(1)       | the same              | ring buffer push, sliding window update |
| O(log n)   | one step longer       | binary search (ex. 6) |
| O(n)       | 2× longer             | one pass, BFS over a grid |
| O(n log n) | a bit more than 2×    | sort + sweep (ex. 2) |
| O(n²)      | 4× longer             | all pairs, "+1" vector growth (ex. 4) |

For n = 1 000 000: n log n ≈ 2·10⁷ operations (milliseconds), n² = 10¹²
(hours). Picking the right algorithm makes that difference.

## Files the programs create

`05_image` writes `image.pgm`, `filled.pgm` and `session.log` into the folder
you run it from (`cpp/`). They are in `.gitignore`.

Rules as in the root `README.md`: predict before running, zero warnings, keep
the experiments and the commented-out code that must not compile (with the
error message), keep the given signatures, leave the asserts in.
