cmake_minimum_required(VERSION 3.16)
project(ChessEngine VERSION 1.0.0 LANGUAGES CXX)

# Force C++20 standard (potrzebne do std::popcount, std::countr_zero itp.)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flagi kompilacji dla maksymalnej wydajności (Release)
# -march=native pozwala kompilatorowi użyć instrukcji popcnt/bmi2 Twojego procesora
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Oi /Ot /GL /DNDEBUG")
endif()

# 1. Rdzeń silnika (wszystkie pliki logiczne bez main.cpp)
# Dzięki temu kod źródłowy skompiluje się raz i będzie użyty w aplikacji i testach
add_library(engine_core STATIC
    src/Board.cpp
    # src/Attacks.cpp
    # src/MoveGenerator.cpp
    # src/Search.cpp
    # src/Evaluate.cpp
)

# Definiujemy, gdzie szukać nagłówków .h dla tej biblioteki
target_include_directories(engine_core PUBLIC src)

# 2. Główny program wykonywalny (silnik szachowy z main.cpp)
add_executable(chess_engine src/main.cpp)

# Łączymy program główny z rdzeniem silnika
target_link_libraries(chess_engine PRIVATE engine_core)

# 3. Włączenie wsparcia dla testów i dodanie podkatalogu tests/
enable_testing()
add_subdirectory(tests)