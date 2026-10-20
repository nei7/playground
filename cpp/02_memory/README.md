# 02_memory — values, references, lifetime, ownership

Prerequisite: `01_basics/` (functions, `std::string`, `std::vector`, structs).

Files `01`–`07` are the exercises from the conversation about memory. Each has its
description in a comment at the top and an empty `main`. The order is not accidental:
each one uses a concept from the previous one.

| File                          | Topic                                               |
|-------------------------------|-----------------------------------------------------|
| `01_destructors.cpp`          | when destructors run: every exit from a scope       |
| `02_value_reference_pointer.cpp` | copy vs alias vs address; sizeof of each          |
| `03_dangling_view.cpp`        | a view that outlives its owner; SSO; reading an ASan report |
| `04_vector_reallocation.cpp`  | pointers into a vector die when it grows            |
| `05_heap.cpp`                 | new/delete: leak, double free, use after free; why vector instead |
| `06_unique_ptr.cpp`           | exclusive ownership, std::move, deleted copy        |
| `07_raii_file.cpp`            | your own RAII wrapper around FILE*; rule of three/five |

Rules as in the root `README.md`: predict before running, zero warnings, leave
the experiments (and the commented-out code that must not compile) in the file.
