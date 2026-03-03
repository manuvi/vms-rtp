<!-- SPDX-License-Identifier: LGPL-2.1-only -->

# vms-rtp

`vms-rtp` is a C++ library for parsing and building RTP packets.
It provides `PacketView` to read/write RTP header and payload fields on existing byte buffers.

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
