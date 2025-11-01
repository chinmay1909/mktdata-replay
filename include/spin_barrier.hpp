#pragma once
#include <atomic>
#include <thread>

class SpinBarrier {
public:
    explicit SpinBarrier(int count): count_(count) {}
    void wait() {
        auto old = count_.fetch_sub(1, std::memory_order_acq_rel);
        if (old == 1) ready_.store(true, std::memory_order_release);
        else while (!ready_.load(std::memory_order_acquire))
            std::this_thread::yield();
    }
private:
    std::atomic<int> count_;
    std::atomic<bool> ready_{false};
};
