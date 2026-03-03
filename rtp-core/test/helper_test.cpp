#include <rtp-core/helper.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

} // namespace

int main()
{
    using namespace vms::rtp;

    expect(byteswap<std::uint16_t>(0x1234u) == 0x3412u, "byteswap u16 failed");
    expect(byteswap<std::uint32_t>(0x11223344u) == 0x44332211u, "byteswap u32 failed");
    expect(byteswap<std::uint64_t>(0x0102030405060708ull) == 0x0807060504030201ull, "byteswap u64 failed");
    expect(byteswap<std::uint8_t>(0x5au) == 0x5au, "byteswap u8 failed");

    constexpr std::uint32_t kSample32 = 0x1234abcdU;
    expect(be_to_host(host_to_be(kSample32)) == kSample32, "be/host roundtrip failed");

    std::array<std::byte, 6> buf{
        std::byte{0x12},
        std::byte{0x34},
        std::byte{0xab},
        std::byte{0xcd},
        std::byte{0x00},
        std::byte{0x00},
    };

    std::uint16_t u16 = 0;
    std::uint32_t u32 = 0;
    expect(read_u16_be(std::span<const std::byte>(buf), 0, u16), "read_u16_be failed");
    expect(u16 == 0x1234u, "read_u16_be value failed");
    expect(read_u32_be(std::span<const std::byte>(buf), 0, u32), "read_u32_be failed");
    expect(u32 == 0x1234abcdu, "read_u32_be value failed");

    std::array<std::byte, 6> out{};
    expect(write_u16_be(std::span<std::byte>(out), 0, 0x89abu), "write_u16_be failed");
    expect(write_u32_be(std::span<std::byte>(out), 2, 0xcdef0123u), "write_u32_be failed");

    expect(out[0] == std::byte{0x89}, "write_u16_be byte0 failed");
    expect(out[1] == std::byte{0xab}, "write_u16_be byte1 failed");
    expect(out[2] == std::byte{0xcd}, "write_u32_be byte2 failed");
    expect(out[3] == std::byte{0xef}, "write_u32_be byte3 failed");
    expect(out[4] == std::byte{0x01}, "write_u32_be byte4 failed");
    expect(out[5] == std::byte{0x23}, "write_u32_be byte5 failed");

    expect(!read_u16_be(std::span<const std::byte>(buf), 5, u16), "read_u16_be out_of_range should fail");
    expect(!read_u32_be(std::span<const std::byte>(buf), 3, u32), "read_u32_be out_of_range should fail");
    expect(!write_u16_be(std::span<std::byte>(out), 5, 0x1111u), "write_u16_be out_of_range should fail");
    expect(!write_u32_be(std::span<std::byte>(out), 4, 0x22222222u), "write_u32_be out_of_range should fail");

    return 0;
}
