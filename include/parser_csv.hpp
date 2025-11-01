#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <stdexcept>

// Basic event structure for replay
struct Event {
    uint64_t ts_ns;
    std::string symbol;
    double price;
    int qty;
    char side;   // 'B' / 'S'
    std::string type; // TRADE / QUOTE
};

class CsvParser {
public:
    static std::vector<Event> load(const std::string& path, size_t max_rows = 0) {
        std::ifstream f(path);
        if (!f.is_open())
            throw std::runtime_error("CsvParser: cannot open file " + path);

        std::vector<Event> out;
        out.reserve(1024);
        std::string line;
        bool header_skipped = false;

        while (std::getline(f, line)) {
            if (!header_skipped && line.find("timestamp") != std::string::npos) {
                header_skipped = true;
                continue;
            }
            std::stringstream ss(line);
            std::string tok;
            Event e{};
            std::getline(ss, tok, ','); e.ts_ns = std::stoull(tok);
            std::getline(ss, e.symbol, ',');
            std::getline(ss, tok, ','); e.price = std::stod(tok);
            std::getline(ss, tok, ','); e.qty = std::stoi(tok);
            std::getline(ss, tok, ','); e.side = tok.empty() ? ' ' : tok[0];
            std::getline(ss, e.type, ',');

            out.push_back(std::move(e));
            if (max_rows && out.size() >= max_rows) break;
        }
        return out;
    }
};
