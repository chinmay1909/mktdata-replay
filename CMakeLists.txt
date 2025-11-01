cmake_minimum_required(VERSION 3.20)
project(mktdata-replay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library
add_library(replay STATIC
    src/parser_csv.cpp
    src/replay_engine.cpp
)

# Executables
add_executable(replay_main src/main.cpp)
add_executable(bench_replay bench/bench_replay.cpp)

target_link_libraries(replay_main PRIVATE replay)
target_link_libraries(bench_replay PRIVATE replay)
