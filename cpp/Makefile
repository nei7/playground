# Every NN_name.cpp in a topic folder (01_basics/, 02_memory/, ...) is a
# separate, single-file program. Targets use the file name WITHOUT the folder:
#
#   make 01_types                compiles 01_basics/01_types.cpp to bin/01_types
#   make run-01_types            compiles and runs
#   make all                     compiles all files
#   make run-05_heap SAN=0       without sanitizers: see how UB "works" silently
#   make format                  clang-format on all files
#   make clean                   removes bin/
#
# File names must be unique across folders (bin/ is flat); make refuses to
# build otherwise. Flags are the same as in cdb (zero warnings is the norm).
# Every build also generates compile_commands.json, from which clangd
# in VS Code reads the flags and header paths.

# CXX is a built-in make variable (defaults to g++, i.e. the compiler from the nix shell).
# Override: `make CXX=clang++ ...`, if you ever add clang to the flake.
SAN    ?= 1

CXXSTD := -std=c++23
WARN   := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion \
          -Wnon-virtual-dtor -Wold-style-cast
DEBUG  := -g -O0

ifeq ($(SAN),1)
  SANFLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer
endif

CXXFLAGS := $(CXXSTD) $(WARN) $(DEBUG) $(SANFLAGS)

# Environment variables for the sanitizers at run time:
#   detect_stack_use_after_return  catches a reference to a local variable after its death
#   print_stacktrace               UBSan shows the stack, not just a single line
RUN_ENV := ASAN_OPTIONS=detect_stack_use_after_return=1 UBSAN_OPTIONS=print_stacktrace=1

# Sources: the root (00_hello.cpp) and one level of topic folders.
SRCS  := $(wildcard *.cpp */*.cpp)
NAMES := $(notdir $(SRCS:.cpp=))
BINS  := $(addprefix bin/,$(NAMES))

# `bin/%: %.cpp` below finds the source by its bare name in any of these folders.
vpath %.cpp $(sort $(dir $(SRCS)))

# Two folders with the same file name would silently build only one of them.
DUPS := $(sort $(foreach n,$(NAMES),$(if $(filter-out 1,$(words $(filter $(n),$(NAMES)))),$(n))))
ifneq ($(DUPS),)
  $(error file names must be unique across folders; duplicates: $(DUPS))
endif

# Full path of the compiler (/nix/store/...), so clangd can ask it
# about system headers (--query-driver in .vscode/settings.json).
CXX_ABS := $(shell command -v $(CXX))

.PHONY: all clean format help FORCE $(NAMES)

all: $(BINS)

# `make 01_types` is a shortcut for `make bin/01_types`.
$(NAMES): %: bin/%

bin/%: %.cpp bin/.flags | bin compile_commands.json
	$(CXX) $(CXXFLAGS) $< -o $@

# Make only compares timestamps, so `SAN=0` after a SAN=1 build would say
# "Nothing to be done" and keep the ASan binary. This file holds the flags of
# the last build and is rewritten only when they change, which then makes
# every binary out of date.
bin/.flags: FORCE | bin
	@echo '$(CXX) $(CXXFLAGS)' | cmp -s - $@ || echo '$(CXX) $(CXXFLAGS)' > $@

FORCE:

run-%: bin/%
	@echo "--- $< ---"
	@$(RUN_ENV) ./$<

bin:
	mkdir -p bin

# Compilation database for clangd. Refreshes whenever a new .cpp file
# appears or the Makefile changes.
compile_commands.json: Makefile $(SRCS)
	@{ echo '['; \
	   first=1; \
	   for f in $(SRCS); do \
	     n=$${f##*/}; n=$${n%.cpp}; \
	     if [ $$first = 1 ]; then first=0; else echo ','; fi; \
	     printf '  {"directory": "%s", "file": "%s", "command": "%s %s %s -o bin/%s"}' \
	       "$(CURDIR)" "$$f" "$(CXX_ABS)" "$(CXXFLAGS)" "$$f" "$$n"; \
	   done; \
	   echo; echo ']'; } > $@
	@echo "refreshed $@ (clangd)"

format:
	clang-format -i $(SRCS)

clean:
	rm -rf bin

help:
	@sed -n '2,14p' Makefile | sed 's/^# \{0,1\}//'
