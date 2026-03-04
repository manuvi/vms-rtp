#include <rtp-core/utils.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "random_test failed: " << message << '\n';
        std::exit(1);
    }
}

template <typename T>
void expect_in_full_range(T value, const char* message)
{
    expect(value >= std::numeric_limits<T>::min(), message);
    expect(value <= std::numeric_limits<T>::max(), message);
}

} // namespace

int main()
{
    using namespace vms::rtp;

    Random rnd;

    // Smoke test on multiple integral types.
    const std::int16_t i16 = rnd.next<std::int16_t>();
    expect_in_full_range(i16, "next<int16_t>() produced out-of-range value");

    const std::uint16_t u16 = rnd.next<std::uint16_t>();
    expect_in_full_range(u16, "next<uint16_t>() produced out-of-range value");

    const std::int32_t i32 = rnd.next<std::int32_t>();
    expect_in_full_range(i32, "next<int32_t>() produced out-of-range value");

    const std::uint64_t u64 = rnd.next<std::uint64_t>();
    expect_in_full_range(u64, "next<uint64_t>() produced out-of-range value");

    // Verify generator state advances by requiring at least one value change
    // across multiple draws.
    const std::uint32_t first = rnd.next<std::uint32_t>();
    bool changed = false;
    for (int i = 0; i < 64; ++i) {
        if (rnd.next<std::uint32_t>() != first) {
            changed = true;
            break;
        }
    }
    expect(changed, "random sequence did not change across draws");

    return 0;
}
