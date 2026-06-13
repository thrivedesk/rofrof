# RofRof

A Pusher-protocol WebSocket server — public / private / presence channels,
client events, and the HTTP push + channel-info API — built on
[uWebSockets](https://github.com/uNetworking/uWebSockets).

### Development

Standard: C++20

### Dependencies

Project dependencies are fetched into a gitignored `third_party/` directory by
CMake's `FetchContent`, so no system `-dev` packages are required:

* uWebSockets `v18.15.0`
* uSockets `7dea1e0` (built with the epoll backend + OpenSSL)
* jsoncpp `1.9.5`

Located from the system (present on any normal toolchain):

* OpenSSL (`libssl` / `libcrypto`)
* zlib

### Build

Requires `cmake` (>= 3.16) and a C++20 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

or via the Makefile wrapper:

```bash
make          # RelWithDebInfo build
make release  # optimized build
```

### Run

```bash
./build/rofrof      # run from the repo root so apps.json is found
# or
make run
```

The server listens on port `7000` and reads its app definitions from
`apps.json` in the working directory.

### Threading

The server runs on a single uWebSockets event loop that owns all connection and
channel state. Each connection is pinned to the loop that accepted it, so
channel fan-out and HTTP-triggered broadcasts all execute on the same loop.

### Features

- [x] App support
- [x] Client messages
- [x] Private channels
- [x] Presence channels
- [x] HTTP Channels API
- [x] HTTP Channel API
- [x] HTTP Users API
- [x] HTTP Trigger API
- [x] Signature verification (constant-time)
- [x] Per-app connection limit
- [ ] Hostname check
- [ ] Path check
- [ ] Cluster support
- [ ] Webhook support
- [ ] Tests
- [ ] Benchmarks
