# Bomberman — Advanced Programming Project 2025-2026

**Name:** TODO
**Student number:** TODO
**GitHub repo:** TODO — paste the link here (required, see section 4.3 "Submitting")

## Building

```
mkdir build && cd build
cmake ..
cmake --build .
```

Requires SFML 2.6.x, CMake ≥ 3.28, and a C++17 compiler. This skeleton was
verified to configure and build with zero warnings (`-Wall -Wextra
-Wpedantic`) on the exact reference lab platform: Ubuntu 24.04, SFML 2.6.1,
CMake 3.28.3, G++ 13.

## Running the tests

```
cd build
ctest --output-on-failure
```

## Running the game

```
cd build
./representation/bomberman
```

## Project layout

```
logic/            SFML-free game-logic static library (bomberman_logic)
  include/logic/  public headers
  src/            implementation
representation/  SFML-based views, factory, states and the Game class
  include/representation/
  src/
tests/            small, dependency-free tests for the logic library
assets/spritesheets/   put your spritesheet(s) here — see the README there
.circleci/config.yml   CI build (install deps, cmake, build, ctest)
```

See section 3.1 ("Code Design") for *why* logic/ and representation/ are two
separate CMake targets: logic/ must stay compilable and testable without
SFML installed at all.

## Design decisions

TODO: document your architecture and any deviations from this skeleton
here, as required in section 4.2 "Core Functionality" (20%) — include your
class diagrams too.

## Extensions implemented

TODO: list and document any bonus extensions from section 2.3 you
implemented (sounds, smarter bots, extra power-ups, alternative build
system, multi-threading, generic programming, additional design patterns, ...).
