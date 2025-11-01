#include "spsc_queue.hpp"
#include "timestamp.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

struct Tick { uint64_t ts; int id; };

int main(int argc, char** argv) {
    size_t iters = 1'000'000;
    size_t qcap = 65'536;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--iters" && i + 1 < argc) iters = std::stoull(argv[++i]);
        else if (a == "--queue-cap" && i + 1 < argc) qcap = std::stoull(argv[++i]);
    }

    SpscQueue<Tick> q(qcap);
    std::atomic<bool> start{false};
    std::atomic<size_t> produced{0}, consumed{0};

    auto producer = [&]() {
        while (!start.load(std::memory_order_acquire));
        for (size_t i = 0; i < iters; ++i) {
            Tick t{now_ns(), static_cast<int>(i)};
            while (!q.try_push(t));
            produced.fetch_add(1, std::memory_order_relaxed);
        }
    };

    auto consumer = [&]() {
        Tick tmp{};
        while (!start.load(std::memory_order_acquire));
        while (consumed.load(std::memory_order_relaxed) < iters) {
            if (q.try_pop(tmp)) consumed.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::thread tp(producer);
    std::thread tc(consumer);
    start.store(true, std::memory_order_release);

    const auto t0 = std::chrono::high_resolution_clock::now();
    tp.join(); tc.join();
    const auto t1 = std::chrono::high_resolution_clock::now();

    const double elapsed = std::chrono::duration<double>(t1 - t0).count();
    const double throughput = static_cast<double>(iters) / elapsed / 1e6;

    std::cout << "Produced: " << produced << ", Consumed: " << consumed << "\n";
    std::cout << "Elapsed: " << elapsed << " s  (" << throughput << " M events/s)\n";
}
