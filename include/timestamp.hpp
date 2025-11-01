#pragma once
#include <chrono>
#include <cstdint>
#include <thread>

using clock_steady = std::chrono::steady_clock;
using ns = std::chrono::nanoseconds;

// Return current time in nanoseconds since steady_clock epoch
inline uint64_t now_ns() noexcept {
    return std::chrono::duration_cast<ns>(clock_steady::now().time_since_epoch()).count();
}

// Busy-wait until target nanosecond timestamp (for pacing)
inline void busy_wait_until(uint64_t target_ns) noexcept {
    while (now_ns() < target_ns) {
        if (target_ns - now_ns() > 2000) // 2 µs guard
            std::this_thread::yield();
    }
}
