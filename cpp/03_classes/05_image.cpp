// Exercise 5 — the rule of zero, 2D grids and BFS: an image editor and a robot vacuum.
//
// SCENARIO
//   Exercise 4 needed five special members and an idiom to get one class
//   right. The professional answer is to not own raw resources in ordinary
//   classes at all: wrap each resource ONCE in a small RAII type
//   (std::vector, std::string, std::unique_ptr, or your own), build
//   everything else from those, and write no special members. That's the
//   rule of zero.
//   You apply it to a grayscale Image (a 2D grid of bytes) that you can
//   save and open in an image viewer, then run real grid algorithms on it:
//   the paint-bucket tool from every image editor (flood fill), counting
//   separate shapes (connected components), and shortest paths for a robot
//   vacuum on a floor plan (breadth-first search).
//
// YOU WILL LEARN
//   C++
//   - the rule of zero: a class whose members manage themselves needs no
//     destructor, copy or move operations. The generated ones are correct.
//   - generated members work member by member: the class is copyable if all
//     members are, and move-only if any member is move-only.
//   - std::unique_ptr with a custom deleter wraps any C handle (FILE*,
//     sockets, library handles) in one line.
//   - explicit operator bool: `if (file)` works, `int n = file;` doesn't.
//   - static_assert + <type_traits>: check class properties at compile time.
//   - C++23 multidimensional subscript: img[x, y].
//   Data structures & algorithms
//   - a 2D grid stored as a 1D array, row by row: index = y * width + x.
//   - graph search on a grid: each pixel is a node, its 4 neighbours are edges.
//   - DFS by recursion overflows the call stack on big inputs; BFS with an
//     explicit queue (std::queue) doesn't.
//   - BFS visits nodes in order of distance, so it finds shortest paths in
//     unweighted graphs (the grid).
//   - connected components: run the fill from every unvisited cell.
//
// STEPS
// 1. Image with zero special members. Write
//      class Image {
//        public:
//          Image(int width, int height);   // all 0 (black); w or h <= 0: std::invalid_argument
//          int width() const;
//          int height() const;
//          bool contains(int x, int y) const;                    // 0 <= x < width, 0 <= y < height
//          std::uint8_t& operator[](int x, int y);               // C++23: two arguments
//          const std::uint8_t& operator[](int x, int y) const;
//          void fill_rect(int x, int y, int w, int h, std::uint8_t value);  // skips outside pixels
//          void save_pgm(const std::string& path) const;
//        private:
//          std::size_t index(int x, int y) const;   // y * width_ + x, as std::size_t
//          int width_;
//          int height_;
//          std::vector<std::uint8_t> pixels_;       // width_ * height_ bytes, row by row
//      };
//    Write NO destructor, copy or move members. In the constructor:
//    pixels_(static_cast<std::size_t>(width) * static_cast<std::size_t>(height))
//    in the initializer list zero-fills the vector. (Mind the declaration
//    order from exercise 2, step 7: pixels_ is declared last, so width_ and
//    height_ are already set when it's built.) index() is the only place
//    with casts; -Wsign-conversion will tell you where they're needed.
//    Then paste the tests that broke IntArray in exercise 4:
//      Image a{4, 3};
//      Image b = a;
//      b[0, 0] = 255;
//      assert(a[0, 0] == 0);    // deep copy for free
//      a = a;                   // self-assignment: fine
//      Image c = std::move(b);  // move: fine
//    ASan is quiet. vector already does deep copy, self-assignment and moves.
//
// 2. Prove it at compile time. Below the class:
//      static_assert(std::is_copy_constructible_v<Image>);
//      static_assert(std::is_nothrow_move_constructible_v<Image>);
//      static_assert(std::is_nothrow_move_assignable_v<Image>);
//    The compiler checks these while compiling; a false one stops the build.
//    is_nothrow_move_constructible is exactly what std::vector checks before
//    deciding to move (exercise 3, step 4).
//    Experiment: add an empty `~Image() {}`. Which static_assert fails and
//    why? (Exercise 3, step 6.) Remove it.
//
// 3. Something to look at. save_pgm writes the plain PGM format:
//      P2
//      <width> <height>
//      255
//      <pixel values separated by spaces, one image row per line>
//    Use std::ofstream (<fstream>): an RAII file like your File from
//    02_memory/07, no close() needed. If `!out` after opening, throw
//    std::runtime_error. Careful: `out << pixel` with a std::uint8_t prints
//    a CHARACTER, not a number. Print `int{pixel}` instead.
//    In main: a 256x128 image with a horizontal gradient (pixel value = x),
//    a white (255) 40x40 square at (30, 40) and a black (0) 40x40 square at
//    (180, 40). Save to "image.pgm" and open it (`xdg-open image.pgm`, or any
//    image viewer). *.pgm is in .gitignore.
//
// 4. The paint bucket: flood fill with BFS. Write a free function
//      int flood_fill(Image& img, int x, int y, std::uint8_t new_value);
//    It recolors the region of pixels connected to (x, y) (up/down/left/right)
//    that have the same value as (x, y), and returns how many pixels it changed.
//    Algorithm (breadth-first search):
//      old = img[x, y]; if old == new_value return 0
//      std::queue<std::pair<int, int>> todo;   // <queue>
//      color (x, y), push it
//      while todo is not empty:
//          take the front (todo.front(), todo.pop()), count it
//          for each of the 4 neighbours: if contains() and value == old:
//              color it NOW (so it's never pushed twice), push it
//    Test on a small image:
//      Image img{8, 6};
//      img.fill_rect(2, 0, 1, 6, 255);    // two vertical white lines
//      img.fill_rect(5, 0, 1, 6, 255);    // split the image into 3 areas
//      assert(flood_fill(img, 0, 0, 100) == 12);
//      assert(flood_fill(img, 0, 0, 100) == 0);   // already 100
//      assert(flood_fill(img, 3, 3, 50) == 12);
//      assert(img[7, 5] == 0);                    // the third area untouched
//    Then use it on the step 3 picture: on a copy, paint the white square
//    gray: assert(flood_fill(copy, 35, 45, 128) == 1600), save as
//    "filled.pgm", look at both files.
//
// 5. Why not recursion. Write the "obvious" version:
//      int flood_fill_recursive(Image& img, int x, int y,
//                               std::uint8_t old_value, std::uint8_t new_value);
//      // if !contains or value != old_value: return 0
//      // color it; return 1 + the four recursive calls
//    It passes the step 4 tests. Now run it on a fresh 1000x1000 black image.
//    Read the report: "AddressSanitizer: stack-overflow" with the same
//    function repeated thousands of times. One call per pixel is up to a
//    million nested calls; the stack (8 MB by default, check `ulimit -s`)
//    holds far fewer. The queue lives on the heap and has no such limit.
//    Run the BFS version on the same image: 1000000.
//    Keep the recursive call commented out with a note.
//    -> A5: what is the maximum recursion depth for an n x n image, and
//       what is the maximum queue size in BFS (roughly)? Which grows with
//       the image area and which only with its "diagonal"?
//
// 6. Connected components: count the shapes.
//      int count_regions(Image img, std::uint8_t value);   // by value: it paints its own copy
//    Scan all pixels; at every pixel that still has `value`, count one
//    region and flood_fill it with a different value (e.g. value + 1).
//    For the 8x6 test image from step 4 (before filling): regions of 0 = 3,
//    regions of 255 = 2. This is how "count the objects in a photo", "count
//    the islands on a map" and "how many separate networks" are solved.
//
// 7. The robot vacuum: shortest path with BFS. Paste this floor plan above
//    main ('#' wall, '.' floor; (x, y) with x = column, y = row):
//      const std::vector<std::string> floor_plan = {
//          "################",
//          "#......#.......#",
//          "#......#.......#",
//          "#..............#",
//          "########.#######",
//          "#......#.......#",
//          "#......#.......#",
//          "#......#########",
//          "###.####.......#",
//          "#......#.......#",
//          "################",
//      };
//    a) `Image from_plan(const std::vector<std::string>& plan)`: '#' -> 255,
//       '.' -> 0. Assert count_regions(..., 0) == 3: three areas no door
//       connects.
//    b) `int distance(const Image& map, int from_x, int from_y, int to_x, int to_y)`:
//       number of steps (up/down/left/right through 0 pixels) on the
//       shortest route, or -1 if there is none. Same BFS as step 4, but
//       instead of coloring, keep a `std::vector<int> dist(width * height, -1)`
//       (use index(x, y), or the same formula); the start gets 0, each
//       neighbour you push gets dist[current] + 1. Stop when you pop the target.
//       The charger is at (1, 1):
//         assert(distance(map, 1, 1, 1, 1) == 0);
//         assert(distance(map, 1, 1, 14, 6) == 18);
//         assert(distance(map, 1, 1, 1, 9) == -1);    // separate area
//         assert(distance(map, 1, 1, 9, 9) == -1);    // the closed storeroom
//    c) The area the robot can clean from the charger: flood fill a copy from
//       (1, 1) and assert it returns 55.
//    -> A7: why does BFS give the SHORTEST path, and DFS (recursion) doesn't?
//       (Think about the order in which cells leave the queue.)
//
// 8. A move-only member: File in one line of resource management.
//    Rewrite your File from 02_memory/07 with zero special members:
//      struct FileCloser {
//          void operator()(std::FILE* f) const noexcept { std::fclose(f); }
//      };
//      class File {
//        public:
//          File(const char* path, const char* mode);   // fopen fails: std::runtime_error
//          void write(std::string_view text);          // std::fwrite(text.data(), 1, size, ...)
//          explicit operator bool() const;             // handle_ != nullptr
//        private:
//          std::unique_ptr<std::FILE, FileCloser> handle_;
//      };
//    When handle_ dies, unique_ptr calls FileCloser{}(ptr) instead of delete
//    (and skips the call for nullptr). operator() is what makes FileCloser
//    callable like a function. handle_.get() gives the raw FILE* for the C calls.
//    Check with static_assert: File is NOT copy constructible (use !) and IS
//    nothrow move constructible. You wrote none of that: unique_ptr's
//    properties carried over.
//    Test: write two lines to "session.log", `File g = std::move(f);`, write
//    a third through g. Then:
//      assert(!f);   assert(g);           // explicit operator bool in a condition
//    and note the errors of `bool ok = g;` and `int n = g;` (explicit: no
//    silent conversion), while `if (g)` and `static_cast<bool>(g)` work.
//    Check the file with `cat session.log`.
//    -> A8: count the lines of resource management in your 02_memory/07 File
//       and in this one.
//
// 9. The decision rule. -> A9: in your own words: when do you write special
//    members at all? Then classify: Account (ex. 1), TimeOfDay (ex. 2),
//    IntArray (ex. 4), Image (here), File (02_memory/07), File (here) —
//    each one: rule of zero, or rule of five, and why.
//
// DONE WHEN
//   [ ] Image has no destructor, copy or move members, and the step 1
//       tests pass under ASan
//   [ ] image.pgm and filled.pgm open in an image viewer
//   [ ] all asserts in steps 4, 6, 7 pass; the recursion crash is noted
//   [ ] File passes its static_asserts; session.log has three lines
//   [ ] A5-A9 answered, zero warnings
//   [ ] you can explain: rule of zero, why BFS uses a queue, why it finds
//       shortest paths, and how a 2D grid maps to a 1D vector
//
// Concepts: https://en.cppreference.com/w/cpp/language/rule_of_three#Rule_of_zero
//           https://en.cppreference.com/w/cpp/memory/unique_ptr
//           https://en.cppreference.com/w/cpp/header/type_traits
//           https://en.cppreference.com/w/cpp/container/queue
//           https://en.wikipedia.org/wiki/Flood_fill
//           https://en.wikipedia.org/wiki/Breadth-first_search
//           https://netpbm.sourceforge.net/doc/pgm.html

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <memory>
#include <print>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

int main() {
    // TODO
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A5:
// A7:
// A8:
// A9:
//   Account:
//   TimeOfDay:
//   IntArray:
//   Image:
//   File (02_memory/07):
//   File (here):
