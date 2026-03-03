#include <vms-rtp/RtpPacket.h>

#include <array>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <stdexcept>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "rtp_packet_setters_test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main()
{
    using namespace vms::rtp;

    // constructor must reject too-small buffers
    {
        bool threw = false;
        std::array<std::byte, 11> small{};
        try {
            PacketView view{std::span<std::byte>(small)};
            (void)view;
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw, "mutable ctor should throw on too-small buffer");

        threw = false;
        try {
            PacketView view{std::span<const std::byte>(small)};
            (void)view;
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw, "const ctor should throw on too-small buffer");
    }

    // header extraction from known buffer
    {
        std::array<std::byte, 16> pkt{
            std::byte{0xB1}, // V=2, P=1, X=1, CC=1
            std::byte{0xE0}, // M=1, PT=96
            std::byte{0x12}, std::byte{0x34}, // sequence
            std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04}, // timestamp
            std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}, std::byte{0xDD}, // ssrc
            std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}, // csrc[0]
        };

        PacketView view{std::span<const std::byte>(pkt)};
        const RtpHeader h = view.header();

        expect(h.version == 2, "header.version mismatch");
        expect(h.padding == true, "header.padding mismatch");
        expect(h.extension == true, "header.extension mismatch");
        expect(h.csrc_count == 1, "header.csrc_count mismatch");
        expect(h.marker == true, "header.marker mismatch");
        expect(h.payload_type == 96, "header.payload_type mismatch");
        expect(h.sequence == 0x1234u, "header.sequence mismatch");
        expect(h.timestamp == 0x01020304u, "header.timestamp mismatch");
        expect(h.ssrc == 0xAABBCCDDu, "header.ssrc mismatch");
        expect(h.csrc.size() == 1, "header.csrc size mismatch");
        expect(h.csrc[0] == 0x11223344u, "header.csrc[0] mismatch");
        expect(view.last_error() == PacketError::none, "last_error should be none after header() success");
    }

    // header extraction failure: declared CSRC list does not fit buffer
    {
        std::array<std::byte, 12> pkt{
            std::byte{0x81}, // V=2, CC=1
            std::byte{0x60},
            std::byte{0x00}, std::byte{0x01},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01},
            std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x02},
        };

        PacketView view{std::span<const std::byte>(pkt)};
        (void)view.header();
        expect(view.last_error() == PacketError::malformed_packet, "malformed_packet expected for truncated CSRC");
    }

    // set_header success + error reset
    {
        std::array<std::byte, 20> pkt{};
        PacketView view{std::span<std::byte>(pkt)};

        RtpHeader h;
        h.version = 2;
        h.padding = false;
        h.extension = false;
        h.csrc_count = 1;
        h.marker = true;
        h.payload_type = 96;
        h.sequence = 7;
        h.timestamp = 0x12345678u;
        h.ssrc = 0x01020304u;
        h.csrc = {0x11223344u};

        expect(view.set_header(h), "set_header should succeed");
        expect(view.last_error() == PacketError::none, "last_error should be none after set_header success");
    }

    // invalid version
    {
        std::array<std::byte, 20> pkt{};
        PacketView view{std::span<std::byte>(pkt)};
        RtpHeader h;
        h.version = 4;
        h.payload_type = 96;
        expect(!view.set_header(h), "set_header should fail for invalid version");
        expect(view.last_error() == PacketError::invalid_version, "invalid_version expected");
    }

    // csrc vector size mismatch
    {
        std::array<std::byte, 20> pkt{};
        PacketView view{std::span<std::byte>(pkt)};
        RtpHeader h;
        h.version = 2;
        h.payload_type = 96;
        h.csrc_count = 1;
        expect(!view.set_header(h), "set_header should fail on csrc size mismatch");
        expect(view.last_error() == PacketError::csrc_size_mismatch, "csrc_size_mismatch expected");
    }

    // destination buffer too small for declared CSRC list
    {
        std::array<std::byte, 12> pkt{};
        PacketView view{std::span<std::byte>(pkt)};
        RtpHeader h;
        h.version = 2;
        h.payload_type = 96;
        h.csrc_count = 1;
        h.csrc = {0x01020304u};
        expect(!view.set_header(h), "set_header should fail for short destination buffer");
        expect(view.last_error() == PacketError::buffer_too_small, "buffer_too_small expected on set_header");
    }

    // set_payload success
    {
        std::array<std::byte, 16> pkt{};
        pkt[0] = std::byte{0x80}; // V=2, no CSRC
        pkt[1] = std::byte{0x60};
        PacketView view{std::span<std::byte>(pkt)};
        std::array<std::byte, 4> payload{std::byte{0x10}, std::byte{0x20}, std::byte{0x30}, std::byte{0x40}};

        expect(view.set_payload(std::span<const std::byte>(payload)), "set_payload should succeed");
        expect(view.last_error() == PacketError::none, "last_error should be none after set_payload success");
        const auto p = view.payload();
        expect(p.size() == 4, "payload size mismatch after set_payload");
        expect(p[0] == std::byte{0x10}, "payload content mismatch 0");
        expect(p[1] == std::byte{0x20}, "payload content mismatch 1");
        expect(p[2] == std::byte{0x30}, "payload content mismatch 2");
        expect(p[3] == std::byte{0x40}, "payload content mismatch 3");
    }

    // set_payload buffer too small
    {
        std::array<std::byte, 12> pkt{};
        pkt[0] = std::byte{0x80}; // header only
        pkt[1] = std::byte{0x60};
        PacketView view{std::span<std::byte>(pkt)};
        std::array<std::byte, 1> payload{std::byte{0x10}};

        expect(!view.set_payload(std::span<const std::byte>(payload)), "set_payload should fail for small buffer");
        expect(view.last_error() == PacketError::buffer_too_small, "buffer_too_small expected");
    }

    return 0;
}
