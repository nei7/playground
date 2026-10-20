// Environment test: `make run-00_hello`.
// If this compiles and runs without warnings, everything works.

#include <print>

int main() {
    std::println("hello, {}! __cplusplus = {}", "playground", __cplusplus);
}
