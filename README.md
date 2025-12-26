# ISO 15118 Wallbox Control System

Electric-vehicle wallbox controller with ISO 15118 simulator support, HTTP API control, and optional CP signal reading for hardware deployments. Current release: **v4.1.0** (see `VERSION`).

Quick links: `docs/guides/QUICK_START.md` · `docs/api/API_REFERENCE.md` · `docs/architecture/ARCHITECTURE_V3.md` · `FILE_STRUCTURE.md`

## Overview

- `wallbox_control_v4` (recommended) provides the REST API, UDP link to the ISO 15118 stack, CP signal handling, and an optional interactive shell.
- `simulator` emulates the ISO 15118 stack and can be driven via CLI commands; it exchanges UDP messages with the controller.
- Protocol structures come from `external/LibPubWallbox`; default ports are 50010/50011 for UDP and 8080 for HTTP.
- C++14 codebase with clean separation between GPIO strategies (stub/hardware), networking, CP signal readers (simulator or hardware), and a charging state machine.
- Legacy binaries (`wallbox_control_v1`..`v3`) remain available for compatibility; use v4 for new work.

## Features

- REST API: enable/disable wallbox, start/stop/pause/resume charging, and status/relay endpoints.
- CP signal reader: simulator mode via UDP or hardware mode via sysfs GPIO (configurable `cp_pin`, default 7).
- GPIO strategies: stub for development, BananaPi/sysfs for production; pins configurable in `config/*.json`.
- Logging: controller entry point writes to `/tmp/wallbox_main.log`, simulator to `/tmp/wallbox_simulator.log`; configuration files also set component log paths (default `/tmp/wallbox_v3.log`).
- Runtime configuration from JSON with environment overrides for `WALLBOX_MODE`, `WALLBOX_API_PORT`, and `WALLBOX_UDP_LISTEN_PORT`.

## Repository Layout

- `src/` – core, api, gpio, network, signal, and simulator sources
- `include/wallbox/` – public headers
- `config/` – `development.json`, `production.json`, `test.json`
- `external/LibPubWallbox/` – ISO 15118 protocol library
- `tests/` – unit and integration tests (GoogleTest optional)
- `scripts/` – build/deploy/test helpers
- `docs/` – guides, architecture, and API reference
- `web/react-app/` – optional dashboard
- `WallboxCtrl/` and `wallbox-portable-deploy/` – legacy/portable build trees kept for compatibility

See `FILE_STRUCTURE.md` for a fuller map.

## Requirements

- CMake ≥ 3.10 and a C++14 compiler (gcc/clang)
- POSIX platform (Linux/macOS); pthreads and sockets available
- `libcurl` for building the simulator (if missing, the simulator target is skipped)
- Optional: GoogleTest for `BUILD_TESTS=ON`

## Build

```bash
cmake -B build -S . -DBUILD_MODE=development   # production|debug also valid
cmake --build build -j
# binaries: build/bin/wallbox_control_v4, simulator, wallbox_control_v1..v3
```

With tests:

```bash
cmake -B build -S . -DBUILD_MODE=development -DBUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build
```

## Run

**Terminal 1 – ISO 15118 simulator**

```bash
./build/bin/simulator
# commands: on | off | idle | ready | charge | stop | status | getudp | setudp <addr> <in> <out> | quit
```

**Terminal 2 – Wallbox controller**

```bash
./build/bin/wallbox_control_v4           # HTTP API only (port 8080)
./build/bin/wallbox_control_v4 --interactive   # CLI only
./build/bin/wallbox_control_v4 --dual           # API + CLI
```

Default ports: controller listens on UDP 50010 and sends on 50011; simulator uses the reverse. Update `config/*.json` or use `setudp` in the simulator to retarget.

### HTTP API (examples)

```bash
curl http://localhost:8080/health
curl http://localhost:8080/api/status
curl -X POST http://localhost:8080/api/wallbox/enable
curl -X POST http://localhost:8080/api/charging/start
```

## Configuration

- Edit `config/development.json` or `config/production.json` to change UDP ports, API port, GPIO pins, charging limits, and log paths. `cp_pin` can be added if hardware CP monitoring is used.
- Environment overrides:
  - `WALLBOX_MODE=development|production|simulator|hardware`
  - `WALLBOX_API_PORT=<port>`
  - `WALLBOX_UDP_LISTEN_PORT=<port>`
- Hardware GPIO access may require root privileges on BananaPi/sysfs systems.

## Docs and Support

- Quick start: `docs/guides/QUICK_START.md`
- Architecture: `docs/architecture/ARCHITECTURE_V3.md`
- API reference: `docs/api/API_REFERENCE.md`
- Troubleshooting and deployment: `docs/guides/INSTALLATION_GUIDE.md` and `docs/guides/MODES_GUIDE.md`

## License

MIT License. See the license text below.

```
Copyright (c) 2025 Achraf Bennani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
```
