#pragma once
#include "timestamp.hpp"
#include "spsc_queue.hpp"
#include "parser_csv.hpp"
#include "spin_barrier.hpp"
#include <thread>
#include <iostream>
#include <atomic>

class ReplayEngine {
public:
    explicit ReplayEngine(std::vector<Event>&& events, double speed = 1.0, uint64_t warmup_ms = 200)
        : events_(std::move(events)), speed_(speed),
          warmup_ns_(warmup_ms * 1'000'000ULL), stop_flag_(false) {}

    void run() {
        std::thread pub(&ReplayEngine::publisher, this);
        pub.join();
    }

    void stop() { stop_flag_.store(true, std::memory_order_release); }

private:
    void publisher() {
        if (events_.empty()) {
            std::cerr << "No events loaded.\n";
            return;
        }
        const uint64_t t0_real = now_ns() + warmup_ns_;
        const uint64_t t0_sim  = events_.front().ts_ns;

        for (const auto& e : events_) {
            if (stop_flag_.load(std::memory_order_acquire)) break;

            uint64_t target_real = t0_real + static_cast<uint64_t>((e.ts_ns - t0_sim) / speed_);
            busy_wait_until(target_real);

            std::cout << e.ts_ns << "," << e.symbol << "," << e.price << ","
                      << e.qty << "," << e.side << "," << e.type << "\n";
        }
    }

    std::vector<Event> events_;
    double speed_;
    uint64_t warmup_ns_;
    std::atomic<bool> stop_flag_;
};
