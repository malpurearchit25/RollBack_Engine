# RollBack Engine

A C++17 rollback simulation engine demonstrating client-side prediction, state snapshots, desync detection, and state rewind, with an interactive browser demo.

## Build and test

Requires CMake 3.16+ and a C++17 compiler (Visual Studio Build Tools, GCC, or Clang).

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

On Windows, build_all.bat performs these steps. Visual Studio places executables under build/Release; other generators normally use build/.

## Run

Run build/rollback_engine (build/Release/rollback_engine.exe with Visual Studio) for a scripted 14-tick rollback scenario. No interactive console input is needed. Authoritative commands arrive three ticks late; output shows corrections and replay.

From the project root, run build/server (build/Release/server.exe with Visual Studio). Open http://localhost:8080 and click Start / Reset simulation. Use WASD or arrows to move, and Space to take 10 damage. Stop the server with Ctrl+C. The current directory must contain web/; CMake also copies it into the build directory.

The browser displays authoritative server state with smooth rendering. It does not run the C++ prediction engine in the browser. The console scenario demonstrates rollback. Server ticks are 500 ms apart; command packets are delayed by 1500 ms.

## Architecture and OOP

- ICommand and concrete commands: abstraction, inheritance, polymorphic state transitions.
- Entity and GameStateSnapshot: encapsulated entity values and independent snapshot copies.
- StateBuffer: shared ownership of history snapshots.
- RollbackManager: mismatch detection, correction, and replay.
- SimulationEngine: composition of the prediction and rollback components.
- StateDesyncException: custom exception carrying mismatch context.
- server_main.cpp and web/index.html: local HTTP simulation and browser display.

Old/main.cpp is an archived entry point and is not built.

## Packet contract and limits

receivePacket accepts contiguous authoritative ticks starting at 1, after the corresponding tick has been simulated. Duplicates, future packets, and gaps return false. Network callers must buffer out-of-order packets and retry them in order. Labels are w, a, s, d, idle, or attackN for a non-negative integer N. Movement repeats; attacks apply once.

The history targets six snapshots but retains the last confirmed base and all pending predictions. Memory may grow while confirmation is stalled. This educational project does not provide production networking, outage recovery, multiple independent players, or cross-platform floating-point determinism guarantees. Verification compares command labels rather than full authoritative state hashes.

## Validation

Regression tests cover delayed corrections, packet ordering, one-shot attacks, invalid damage, and tick counts. GitHub Actions builds and runs tests on Windows and Linux.

## Third-party code

include/httplib.h vendors cpp-httplib and includes its copyright and MIT license notice. A project-wide license has not yet been selected.
