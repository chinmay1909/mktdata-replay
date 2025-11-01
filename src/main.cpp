#include "parser_csv.hpp"
#include "replay_engine.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string file = "data/sample.csv";
    double speed = 1.0;
    uint64_t warmup_ms = 200;
    bool summary = false;

    // simple CLI parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) file = argv[++i];
        else if (arg == "--speed" && i + 1 < argc) speed = std::stod(argv[++i]);
        else if (arg == "--warmup-ms" && i + 1 < argc) warmup_ms = std::stoull(argv[++i]);
        else if (arg == "--summary") summary = true;
    }

    try {
        auto events = CsvParser::load(file);
        if (summary) {
            std::cout << "Loaded " << events.size() << " events from " << file << "\n";
            if (!events.empty()) {
                std::cout << "First: " << events.front().ts_ns
                          << ", Last: " << events.back().ts_ns << "\n";
            }
            return 0;
        }

        ReplayEngine engine(std::move(events), speed, warmup_ms);
        engine.run();
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
