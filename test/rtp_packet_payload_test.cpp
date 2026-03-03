#include <vms-rtp/RtpPacket.h>

#include <array>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "rtp_packet_payload_test failed: " << message << '\n';
        std::exit(1);
    }
}

std::array<std::byte, 12> make_minimal_header(std::uint8_t csrc_count = 0)
{
    std::array<std::byte, 12> header{};
    header[0] = std::byte{static_cast<unsigned char>(0x80u | (csrc_count & 0x0Fu))}; // V=2
    header[1] = std::byte{0x60}; // PT=96
    return header;
}

} // namespace

int main()
{
    using namespace vms::rtp;

    // 1) minimal RTP packet (12 bytes) -> empty payload
    {
        auto pkt = make_minimal_header(0);
        PacketView view{std::span<const std::byte>(pkt)};
        const auto p = view.payload();
        expect(p.size() == 0, "minimal packet payload must be empty");
    }

    // 2) packet with payload -> payload size/content are correct
    {
        std::array<std::byte, 16> pkt{};
        auto h = make_minimal_header(0);
        for (std::size_t i = 0; i < h.size(); ++i) pkt[i] = h[i];
        pkt[12] = std::byte{0x11};
        pkt[13] = std::byte{0x22};
        pkt[14] = std::byte{0x33};
        pkt[15] = std::byte{0x44};

        PacketView view{std::span<const std::byte>(pkt)};
        const auto p = view.payload();
        expect(p.size() == 4, "payload size mismatch");
        expect(p[0] == std::byte{0x11}, "payload byte 0 mismatch");
        expect(p[1] == std::byte{0x22}, "payload byte 1 mismatch");
        expect(p[2] == std::byte{0x33}, "payload byte 2 mismatch");
        expect(p[3] == std::byte{0x44}, "payload byte 3 mismatch");
    }

    // 3) csrc_count > 0 -> payload starts after CSRC list
    {
        std::array<std::byte, 18> pkt{};
        auto h = make_minimal_header(1); // one CSRC => +4 header bytes
        for (std::size_t i = 0; i < h.size(); ++i) pkt[i] = h[i];

        // CSRC bytes 12..15 (any value)
        pkt[12] = std::byte{0xaa};
        pkt[13] = std::byte{0xbb};
        pkt[14] = std::byte{0xcc};
        pkt[15] = std::byte{0xdd};

        // payload starts at 16
        pkt[16] = std::byte{0xee};
        pkt[17] = std::byte{0xff};

        PacketView view{std::span<const std::byte>(pkt)};
        const auto p = view.payload();
        expect(p.size() == 2, "payload size with CSRC mismatch");
        expect(p[0] == std::byte{0xee}, "payload with CSRC byte 0 mismatch");
        expect(p[1] == std::byte{0xff}, "payload with CSRC byte 1 mismatch");
    }

    // 4) malformed packet -> header_size > buffer size => empty payload + error
    {
        auto pkt = make_minimal_header(1); // needs 16 bytes but buffer is only 12
        PacketView view{std::span<const std::byte>(pkt)};
        const auto p = view.payload();
        expect(p.empty(), "malformed packet must produce empty payload");
        expect(view.last_error() == PacketError::malformed_packet, "malformed error expected");
    }

    // 5) both const and mutable views
    {
        std::array<std::byte, 14> pkt{};
        auto h = make_minimal_header(0);
        for (std::size_t i = 0; i < h.size(); ++i) pkt[i] = h[i];
        pkt[12] = std::byte{0x7a};
        pkt[13] = std::byte{0x7b};

        PacketView mutable_view{std::span<std::byte>(pkt)};
        const auto p_mut = mutable_view.payload();
        expect(p_mut.size() == 2, "mutable view payload size mismatch");
        expect(p_mut[0] == std::byte{0x7a}, "mutable view payload byte 0 mismatch");
        expect(p_mut[1] == std::byte{0x7b}, "mutable view payload byte 1 mismatch");

        PacketView const_view{std::span<const std::byte>(pkt)};
        const auto p_const = const_view.payload();
        expect(p_const.size() == 2, "const view payload size mismatch");
        expect(p_const[0] == std::byte{0x7a}, "const view payload byte 0 mismatch");
        expect(p_const[1] == std::byte{0x7b}, "const view payload byte 1 mismatch");
    }

    return 0;
}
