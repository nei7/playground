# 01_basics — the language core

Everything `02_memory/` takes for granted: what a type is and how big, the
initialization forms, control flow, functions and overloading, `std::string`,
`std::vector`, structs and enums, reading stdin. Nothing about pointers or the
heap yet; that is the next folder.

Do the files in order. Each one is a program: `make run-01_types`. Read the
comment at the top, write the code in `main` (helpers above it), run, compare
with what you predicted. Most steps end with a question: answer it in a
comment next to the code.

| File                     | Topic                                      | Done when |
|--------------------------|--------------------------------------------|-----------|
| `01_types.cpp`           | sizes, limits, overflow, signed/unsigned, floating point, narrowing | every step has a printed result and the questions are answered in comments |
| `02_initialization.cpp`  | uninitialized values, brace init, auto, const, constexpr, shadowing | the `static_assert`s in step 4 all pass |
| `03_control_flow.cpp`    | if with initializer, switch, loops, range-for, break/continue | the four functions pass the listed asserts |
| `04_functions.cpp`       | declaration vs definition, parameter passing, overloading, defaults, recursion, namespaces | the overload predictions are written down and compared with reality |
| `05_strings.cpp`         | std::string API, find/npos, substr, conversions, string_view, std::format | `is_palindrome`, `to_upper`, `trim`, `split` pass their asserts |
| `06_vectors.cpp`         | C array vs std::array vs std::vector, bounds, size_t loops, 2D | the five functions pass their asserts and match the `<algorithm>` versions |
| `07_structs_enums.cpp`   | aggregates, member functions, const, enum class, structured bindings | `Rect` and `Suit` helpers work; the const-correctness errors are recorded |
| `08_stats.cpp`           | capstone: stdin, parsing, std::vector, functions, formatting | the three test invocations print correct statistics; zero warnings |

Rules (root `README.md` has the full list): predict before running, zero
warnings, keep the experiments and the commented-out code that must not
compile, keep the given function signatures, leave the asserts in.
