<!-- SPDX-License-Identifier: LGPL-2.1-only -->

# vms-rtp

`vms-rtp` is now organized as a multi-library C++ project:

- `rtp-core`: core RTP packet parsing/building (`PacketView`, `RtpHeader`).
- `rtp-net`: network bridge layer built on top of `rtp-core`.

## Repository Layout

- `rtp-core/include`, `rtp-core/src`, `rtp-core/private`, `rtp-core/test`
- `rtp-net/include`, `rtp-net/src`, `rtp-net/test`

## Build

### Shared (default)

```bash
cmake -S . -B build
cmake --build build
```

### Static

```bash
cmake -S . -B build-static -DVMS_RTP_BUILD_SHARED=OFF
cmake --build build-static
```

### Build only selected libraries

```bash
# Build only rtp-core
cmake -S . -B build-core -DVMS_RTP_BUILD_CORE=ON -DVMS_RTP_BUILD_NET=OFF
cmake --build build-core

# Build both (default)
cmake -S . -B build-all -DVMS_RTP_BUILD_CORE=ON -DVMS_RTP_BUILD_NET=ON
cmake --build build-all
```

`rtp-net` depends on `rtp-core`, so `VMS_RTP_BUILD_NET=ON` requires `VMS_RTP_BUILD_CORE=ON`.

## Tests

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Coverage

```bash
cmake -S . -B build -DBUILD_TESTING=ON -DVMS_RTP_ENABLE_COVERAGE=ON
cmake --build build --target coverage
```

Coverage outputs:

- `build/coverage.info`
- `build/coverage-html/index.html`

## Copyright

Copyright (c) 2026 Manuel Virgilio `<real_virgil@yahoo.it>`.

## License

This project is licensed under **GNU LGPL v2.1**.
See [LICENSE](LICENSE).
