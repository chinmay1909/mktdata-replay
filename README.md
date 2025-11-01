# Nanosecond Market Data Replay Engine (C++20)

Ultra-low-latency **nanosecond-precision** market data replay engine.  
Streams timestamped events (quotes, trades, book updates) at *realistic wall-clock pace*, enabling reproducible HFT backtests, latency studies, and system soak tests.

---

## 🔑 Highlights

- **C++20, lock-free SPSC queues** (single-producer/single-consumer)  
- **Nanosecond pacing** using steady clock + spin/yield backoff  
- **Cache-friendly data path** (SoA/packed structs, cacheline alignment)  
- **Deterministic replays** with warm-up and fixed seeds  
- **Composable outputs**: file, stdout, UDP (planned), named pipe (planned)

---

## 📂 Repository Structure

```bash
mktdata-replay/
│
├── CMakeLists.txt # build config (C++20, -O3, LTO)
├── README.md # this file
├── LICENSE
│
├── include/
│ ├── spsc_queue.hpp # lock-free ring buffer (cache-aligned)
│ ├── timestamp.hpp # time utilities (ns)
│ ├── parser_csv.hpp # fast CSV parser for events
│ ├── replay_engine.hpp # core replay engine interface
│ └── spin_barrier.hpp # lightweight thread barrier
│
├── src/
│ ├── main.cpp # CLI entrypoint: reads CSV, replays
│ ├── parser_csv.cpp # CSV implementation
│ └── replay_engine.cpp # pacing + publishing loop
│
├── data/
│ └── sample.csv # tiny demo dataset (ts, symbol, px, qty, side, type)
│
└── bench/
└── bench_replay.cpp # micro-benchmark / throughput test
```

```yaml
## ⚙️ Build (CMake)

### Prerequisites
- CMake ≥ 3.20  
- GCC ≥ 11 / Clang ≥ 14 (C++20)  
- Linux / macOS (Windows builds fine with MSVC 2022)

### Commands

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```
## ▶️ Run
### 1️⃣ Replay demo data
./build/replay_main --file data/sample.csv --speed 1.0 --warmup-ms 200


--speed 1.0 → real-time speed

--speed 2.0 → 2× faster

--speed 0.5 → half speed

--warmup-ms → spin warmup before pacing (reduces initial jitter)

### 2️⃣ Summary-only mode
./build/replay_main --file data/sample.csv --summary

### 🧪 Benchmark
./build/bench_replay --iters 1000000 --queue-cap 65536


Prints:

throughput (events/sec)

latency histogram (p50/p99 publish times)

🧱 Event Format (CSV)

timestamp_ns,symbol,price,qty,side,type

Example:

170000000012345678,SPY,456.32,100,B,TRADE
170000000012345980,SPY,456.31,500,S,QUOTE


timestamp_ns — nanoseconds since epoch (int64)

symbol — instrument ID (string)

price, qty — floats/ints

side — B/S

type — TRADE / QUOTE

## 🧠 Design Notes

SPSC ring: one parser thread produces, one publisher thread consumes.

Backoff: busy-spin with std::this_thread::yield() fallback for minimal jitter.

Cache alignment: 64-byte cacheline padding between head/tail counters.

Determinism: event sequencing independent of system clock; only pacing uses it.

Data locality: contiguous SoA layout → minimal false sharing.

## 🗺️ Roadmap

 UDP multicast publisher

 Named pipe / ZeroMQ output

 Multi-symbol partitioning

 PCAP tick-stream import

 FIX / ITCH protocol adapters

## 🤝 Contributing

Pull requests welcome!
Keep changes measurable (benchmark before/after) and cache-aware.