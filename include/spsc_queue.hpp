#pragma once
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <new>
#include <cassert>

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif

// -----------------------------------------------------------------------------
// Single-Producer Single-Consumer Lock-Free Queue (ring buffer)
// -----------------------------------------------------------------------------
template<typename T>
class SpscQueue {
    static_assert(std::is_trivially_copyable_v<T>, "SpscQueue requires trivially copyable types");
public:
    explicit SpscQueue(size_t capacity_pow2)
        : cap_(capacity_pow2), mask_(capacity_pow2 - 1),
          buf_(static_cast<T*>(::operator new[](cap_ * sizeof(T), std::align_val_t{CACHELINE_SIZE}))) {
        assert((capacity_pow2 & (capacity_pow2 - 1)) == 0 && "capacity must be power of two");
    }

    ~SpscQueue() { ::operator delete[](buf_, std::align_val_t{CACHELINE_SIZE}); }

    bool try_push(const T& v) noexcept {
        const auto head = head_.load(std::memory_order_relaxed);
        const auto next = (head + 1) & mask_;
        if (next == tail_.load(std::memory_order_acquire))
            return false;
        buf_[head] = v;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool try_pop(T& out) noexcept {
        const auto tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
            return false;
        out = buf_[tail];
        tail_.store((tail + 1) & mask_, std::memory_order_release);
        return true;
    }

    bool empty() const noexcept { return head_.load() == tail_.load(); }
    size_t capacity() const noexcept { return cap_; }

private:
    alignas(CACHELINE_SIZE) std::atomic<size_t> head_{0};
    alignas(CACHELINE_SIZE) std::atomic<size_t> tail_{0};
    const size_t cap_;
    const size_t mask_;
    T* buf_;
};
