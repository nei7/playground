# playground/cpp — learning C++ from the ground up

Single-file C++23 programs, one topic per folder, compiled with strict warnings
and sanitizers. Same compiler and flags as the `cdb` project, but no CMake:
every `NN_name.cpp` is a separate program.

## Layout

```
cpp/
  00_hello.cpp      environment check
  01_basics/        the language core: types, initialization, control flow,
                    functions, strings, vectors, structs and enums, input/output
  02_memory/        values vs references, lifetime, heap, unique_ptr, RAII
  03_classes/       designing types: invariants, constructors, rule of 0/3/5,
                    operators; plus data structures and algorithms in practice
  Makefile          builds any file by its bare name, no registration needed
  flake.nix .envrc  the toolchain (g++, clangd, gdb, valgrind)
  .vscode/          build / run / debug the open file
```

Each folder has a `README.md` with the list of exercises and the order to do
them in. Each exercise is one file: the task is a numbered comment at the top,
`main` is empty and waiting.

## Curriculum

| #  | Folder            | Topics                                                        | Status  |
|----|-------------------|---------------------------------------------------------------|---------|
| 01 | `01_basics/`      | types, initialization, control flow, functions, std::string, std::vector, struct/enum, I/O | ready |
| 02 | `02_memory/`      | copy vs reference vs pointer, lifetime, dangling, heap, unique_ptr, RAII | ready |
| 03 | `03_classes/`     | constructors, special members, rule of 0/3/5, invariants, operators; DS&A: dynamic array, ring buffer, BFS, binary search, intervals, greedy | ready |
| 04 | `04_move/`        | value categories, move semantics, std::move / std::forward     | planned |
| 05 | `05_templates/`   | function and class templates, lambdas, auto, concepts          | planned |
| 06 | `06_stl/`         | containers, iterators, algorithms, ranges                      | planned |
| 07 | `07_errors/`      | exceptions, noexcept, std::optional, std::expected             | planned |
| 08 | `08_polymorphism/`| inheritance, virtual, interfaces, std::variant as alternative  | planned |
| 09 | `09_compile_time/`| constexpr, consteval, static_assert, type traits               | planned |
| 10 | `10_concurrency/` | std::thread / jthread, mutex, atomics, async                   | planned |

Folders are created when their exercises are written; the order is fixed.

## Usage

Run everything from this directory (`cd playground/cpp`), not from the
sandbox's top-level directory. The flake lives here, and nix finds it on its own
in a subfolder of the git repository.

```sh
direnv allow                 # once; afterwards the shell loads itself
make run-00_hello            # checks that the environment works
make run-01_types            # 01_basics/01_types.cpp: compiles with sanitizers and runs
make run-05_heap SAN=0       # the same without sanitizers
make 08_stats && echo "1 2 3" | ./bin/08_stats     # programs that read stdin
make help                    # remaining commands
```

Targets use the bare file name; make finds the folder on its own. That means
file names must be unique across folders (make refuses to build otherwise).

In VS Code, open **this folder** as the workspace (`code playground/cpp`), because
`.envrc` and `.vscode/` live here. Then `Ctrl+Shift+B` builds the open file,
"Tasks: Run Test Task" builds and runs it, `F5` debugs under gdb.
Extensions as in cdb, without CMake Tools.

## How to work through an exercise

1. Read the whole comment at the top before writing anything.
2. For every step that says "predict", write the prediction down (a comment in
   the file is fine) *before* running. Being wrong is the useful part.
3. Zero warnings. The flags are strict on purpose: a warning is a question, and
   the answer is either a fix or a comment explaining why the code is right.
4. Leave the experiments in the file. Code that must not compile (the exercise
   says so) stays commented out, with a one-line note of what the compiler said:
   `// error: cannot bind non-const lvalue reference ...`. That record is what
   gets reviewed later, so don't tidy it away.
5. Functions with a given signature keep that signature. Test them with
   `assert` from `<cassert>`; leave the asserts in.
6. When the folder is done (or you are stuck on one file), ask for a review.

## New file

Create `NN_topic/NN_name.cpp` with a `main` function and run `make run-NN_name`.
Nothing else needs to be registered. The first `make` generates `compile_commands.json`,
from which clangd takes its flags; if clangd gives no completions after adding a file,
run `make all` or "clangd: Restart language server".

## How to read a sanitizer report

A program with a memory error prints a block starting with `==PID==ERROR: AddressSanitizer:`.
Three sections, from the top:

1. **What and where** — the kind of error (e.g. `heap-use-after-free`) and the call stack
   of the place where the memory was used. Look for the first line with a path to your file.
2. **freed by** — the stack of the place where that memory was freed.
3. **previously allocated by** — the stack of the place where it was allocated.

A memory leak looks different: `LeakSanitizer: detected memory leaks` appears
only on program exit, with a `Direct leak of N byte(s)` section.

UBSan prints a single `runtime error:` line with the file and line number.

Valgrind (used in `01_basics/02_initialization.cpp`) does not mix with the
sanitizers: build with `SAN=0` first, then `valgrind ./bin/NN_name`.
