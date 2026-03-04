/*
 * Copyright (C) 2026 Manuel Virgilio <real_virgil@yahoo.it>
 *
 * This file is part of vms-rtp.
 * Licensed under the GNU Lesser General Public License v2.1.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <rtp-core/RtpHeader.h>

#include <cstddef>
#include <span>


namespace vms::rtp {

enum class PacketError {
    none = 0,
    buffer_too_small,
    malformed_packet,
    packet_const,
    invalid_version,
    invalid_csrc_count,
    invalid_payload_type,
    csrc_size_mismatch,
    max_csrc_reached,
};

class PacketView {
public:
    explicit PacketView(std::span<std::byte> buf);
    explicit PacketView(std::span<const std::byte> buf);

    // getters (from const view)
    RtpHeader header() const noexcept;
    std::size_t header_size() const noexcept;
    std::span<const std::byte> payload() const noexcept;
    PacketError last_error() const noexcept;

    // setters
    bool set_header(const RtpHeader& header) noexcept;
    bool set_payload(std::span<const std::byte> data) noexcept;
    // per parameter setters, to handle fast little variations
    bool set_version(std::uint8_t value) noexcept;
    bool set_padding(bool value) noexcept;
    bool set_extension(bool value) noexcept;
    bool set_marker(bool value) noexcept;
    bool set_payload_type(std::uint8_t value) noexcept;
    bool set_sequence(std::uint16_t value) noexcept;
    bool set_timestamp(std::uint32_t value) noexcept;
    bool set_ssrc(std::uint32_t value) noexcept;
    bool add_csrc(std::uint32_t value) noexcept;
    bool remove_csrc(std::uint32_t value) noexcept;

private:
    static constexpr std::size_t kBaseHeaderSize = 12;
    static constexpr std::size_t kMaxVersion = 3;
    static constexpr std::size_t kMaxCSRC = 15;
    static constexpr std::size_t kMaxPayloadType = 127;

    // getter for bytes
    std::span<const std::byte> bytes() const noexcept;
    bool ensure_mutable() const noexcept;
    bool fail(PacketError error) const noexcept;
    void clear_error() const noexcept;

    // data span from existing buffer
    std::span<const std::byte> cbuf_{};
    std::span<std::byte> buf_{};
    mutable PacketError last_error_{PacketError::none};
};

} // namespace vms::rtp
