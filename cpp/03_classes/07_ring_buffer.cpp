// Exercise 7 — a ring buffer and sliding windows: latency monitoring.
//
// SCENARIO
//   A server monitor records how long each request took (in milliseconds)
//   and shows the average of the last N requests. When that average goes
//   above a limit, it raises an alert, once, and reports when it recovers.
//   Keeping "the last N values" is the job of a ring buffer (circular
//   buffer): a fixed array where new values overwrite the oldest ones. The
//   same structure sits in keyboard drivers, audio playback, network cards,
//   `tail -n`, and every "recent items" list.
//
// YOU WILL LEARN
//   C++
//   - a class whose invariant is about INDICES (head, size, capacity) rather
//     than values, and a private helper that is the only place doing the
//     index arithmetic.
//   - a logical view over a physical layout: operator[](0) is the oldest
//     element wherever it sits in memory.
//   - composition: MovingAverage HAS a RingBuffer and keeps a running sum
//     in step with it (two members, one invariant across both).
//   - rule of zero again: the buffer is a std::vector, nothing to write.
//   Data structures & algorithms
//   - ring buffer / circular queue: push and pop in O(1), no shifting, with
//     modular arithmetic (i + 1) % capacity.
//   - sliding window: update a sum in O(1) per element (add the new, subtract
//     the one that falls out) instead of re-summing the window, O(window).
//   - (bonus) sliding window maximum with a monotonic deque: O(1) amortized.
//   - edge-triggered alerts: react to a CHANGE of state, not to every
//     sample above the limit (a tiny state machine).
//
// STEPS
// 1. The structure. Write
//      class RingBuffer {
//        public:
//          explicit RingBuffer(std::size_t capacity);   // throws std::invalid_argument if 0
//          void push(int value);         // when full, overwrites the OLDEST value
//          int pop();                    // removes and returns the oldest; precondition !empty()
//          std::size_t size() const;
//          std::size_t capacity() const;
//          bool empty() const;
//          bool full() const;
//          int operator[](std::size_t i) const;   // 0 = oldest, size() - 1 = newest
//          int oldest() const;
//          int newest() const;
//        private:
//          std::size_t physical(std::size_t i) const;   // (head_ + i) % capacity()
//          std::vector<int> data_;      // always capacity() elements, allocated once
//          std::size_t head_ = 0;       // where the oldest element is
//          std::size_t size_ = 0;       // how many are in use
//      };
//    Write the invariant above the class: data_.size() >= 1, size_ <= data_.size(),
//    head_ < data_.size(); logical element i is data_[physical(i)].
//    push when not full: write at physical(size_), ++size_.
//    push when full: overwrite data_[head_] (the oldest), advance head_.
//    pop: read data_[head_], advance head_, --size_.
//    Draw it on paper for capacity 3 and pushes 1, 2, 3, 4, 5 before coding:
//    where is head_ after each push?
//      RingBuffer r{3};
//      for (int v : {1, 2, 3, 4, 5}) r.push(v);
//      assert(r.size() == 3 && r.full());
//      assert(r[0] == 3 && r[1] == 4 && r[2] == 5);
//      assert(r.oldest() == 3 && r.newest() == 5);
//      assert(r.pop() == 3 && r.size() == 2);
//      r.push(6);  r.push(7);
//      assert(r[0] == 5 && r[1] == 6 && r[2] == 7);
//    -> A1: why does a ring buffer never need to move elements, while
//       vector.erase(begin()) (removing the oldest) moves all of them?
//
// 2. Moving average, the O(1) way.
//      class MovingAverage {
//        public:
//          explicit MovingAverage(std::size_t window);
//          double add(int latency_ms);   // returns the average of the last `window` values
//        private:
//          RingBuffer window_;
//          long long sum_ = 0;           // invariant: sum_ == sum of everything in window_
//      };
//    add: if the window is full, subtract the value that is about to fall out
//    (window_.oldest()); push; add the new value; return
//    static_cast<double>(sum_) / static_cast<double>(window_.size()).
//    The sum is an integer on purpose: a running sum of doubles drifts as
//    rounding errors accumulate over millions of updates; integers are exact.
//    Feed it these latencies with a window of 5 and an alert limit of 200 ms:
//      std::vector<int> latencies{120, 135, 110, 140, 125, 180, 260, 310,
//                                 290, 275, 150, 130, 120, 115, 125, 118};
//    Print one line per request:
//      std::println("#{:<3}{:>5} ms   avg {:>6.1f} ms{}", n, latency, avg, note);
//    `note` is "  ALERT: average above 200 ms" only on the request where the
//    average crosses the limit upward, "  recovered" only where it comes back
//    to <= 200, otherwise empty. (Keep a `bool alert_active`.) Expected:
//      #1    120 ms   avg  120.0 ms
//      #2    135 ms   avg  127.5 ms
//      #3    110 ms   avg  121.7 ms
//      #4    140 ms   avg  126.2 ms
//      #5    125 ms   avg  126.0 ms
//      #6    180 ms   avg  138.0 ms
//      #7    260 ms   avg  163.0 ms
//      #8    310 ms   avg  203.0 ms  ALERT: average above 200 ms
//      #9    290 ms   avg  233.0 ms
//      #10   275 ms   avg  263.0 ms
//      #11   150 ms   avg  257.0 ms
//      #12   130 ms   avg  231.0 ms
//      #13   120 ms   avg  193.0 ms  recovered
//      #14   115 ms   avg  158.0 ms
//      #15   125 ms   avg  128.0 ms
//      #16   118 ms   avg  121.6 ms
//    (#4 is 126.25 shown as 126.2: std::format rounds an exact half to the
//    even digit.)
//    -> A2: why alert only on the crossing and not on every request above the
//       limit? Think about who reads the alerts.
//
// 3. Measure O(1) vs O(window). Write the naive version:
//      double naive_average(RingBuffer& window, int latency_ms);
//      // push, then sum all elements of the window with a loop
//    Generate 200 000 samples with `100 + (i * 37) % 200` and time both
//    versions for windows of 10, 100 and 1000 (std::chrono as in exercise 4,
//    build with SAN=0). Also sum the returned averages of both and check
//    they're equal (same integer sums, so exactly equal). Reference numbers:
//      window    10: running sum ~5 ms, naive ~20 ms
//      window   100: running sum ~5 ms, naive ~160 ms
//      window  1000: running sum ~5 ms, naive ~1500 ms
//    -> A3: your numbers, and: which version's time depends on the window
//       size, and why?
//
// 4. Sliding window maximum (bonus, a classic interview problem). The
//    monitor also shows the WORST latency of the last N requests. Re-scanning
//    the window is O(N) per request. The O(1) amortized method keeps a
//    std::deque<std::pair<long long, int>> (<deque>) of (request number,
//    latency) whose latencies are DECREASING from front to back:
//      on a new value v at request n:
//        pop from the BACK while back.latency <= v   (they can never be the max again)
//        push (n, v) at the back
//        pop from the FRONT while front.number <= n - window   (fell out of the window)
//        the max is front.latency
//    Write
//      class SlidingMax {
//        public:
//          explicit SlidingMax(std::size_t window);
//          int add(int latency_ms);    // returns the max of the last `window` values
//        ...
//      };
//    and check it against a brute-force max over a RingBuffer for all
//    200 000 samples (assert equal at every step), for windows 1, 5 and 1000.
//    -> A4: each value is pushed once and popped at most once. What does that
//       say about the total work for n values, whatever the window?
//
// 5. `tail`: the last N lines of stdin. Make a second, string version of the
//    buffer: copy RingBuffer to `class LineBuffer` storing std::string.
//    (Copying a whole class to change one type is exactly the problem
//    templates solve: 05_templates. Write down which lines differ.)
//    Then: read stdin line by line into a LineBuffer of capacity 3 and print
//    what's left at the end, oldest first. Test:
//      make 07_ring_buffer && seq 1 10 | ./bin/07_ring_buffer
//    must end with 8, 9, 10 on three lines. `tail -n 3` does exactly this.
//    (Put the stdin part last in main, after all the asserts. Started
//    without a pipe, the program now waits for keyboard input: type a few
//    lines and end with Ctrl+D, or run `make run-07_ring_buffer < /dev/null`.)
//
// DONE WHEN
//   [ ] step 1 asserts pass, including after wrap-around
//   [ ] step 2 output matches exactly
//   [ ] step 3 timings recorded in A3; both versions give equal results
//   [ ] (bonus) SlidingMax agrees with brute force on all samples
//   [ ] `seq 1 10 | ./bin/07_ring_buffer` ends with 8 9 10
//   [ ] A1-A4 answered, zero warnings
//   [ ] you can explain: how head and size locate every element, why push
//       and pop are O(1), and how a sliding-window sum stays O(1)
//
// Concepts: https://en.wikipedia.org/wiki/Circular_buffer
//           https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average
//           https://en.cppreference.com/w/cpp/container/deque
//           https://en.cppreference.com/w/cpp/chrono/steady_clock

#include <cassert>
#include <chrono>
#include <cstddef>
#include <deque>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int main() {
    // TODO
}

// -----------------------------------------------------------------------------
// ANSWERS (one to three sentences each)
// A1:
// A2:
// A3:
// A4:
// A5 (which lines differ between RingBuffer and LineBuffer):
