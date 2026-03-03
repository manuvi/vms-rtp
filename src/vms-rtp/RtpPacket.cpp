/*
 * Copyright (C) 2026 Manuel Virgilio <real_virgil@yahoo.it>
 *
 * This file is part of vms-rtp.
 * Licensed under the GNU Lesser General Public License v2.1.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <vms-rtp/RtpPacket.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

#include <vms-rtp/helper.h>

namespace vms::rtp {

PacketView::PacketView(std::span<std::byte> buf)
    : buf_(buf)
{
    if (buf_.size() < kBaseHeaderSize) throw std::runtime_error("RTP buffer too small");
}

PacketView::PacketView(std::span<const std::byte> buf)
    : cbuf_(buf)
    , buf_(std::span<std::byte>{})
{
    if (cbuf_.size() < kBaseHeaderSize) throw std::runtime_error("RTP buffer too small");
}

std::span<const std::byte> PacketView::bytes() const noexcept
{
    // PacketView can be built from mutable or const storage.
    if ( cbuf_.empty() ) {
        return std::span<const std::byte>(buf_);
    }
    else {
        return cbuf_;
    }
}

PacketError PacketView::last_error() const noexcept
{
    return last_error_;
}

bool PacketView::ensure_mutable() const noexcept
{
    // Mutating operations are valid only when a mutable buffer is owned.
    if (buf_.empty()) return fail(PacketError::packet_const);
    return true;
}

bool PacketView::fail(PacketError error) const noexcept
{
    last_error_ = error;
    return false;
}

void PacketView::clear_error() const noexcept
{
    last_error_ = PacketError::none;
}

RtpHeader PacketView::header() const noexcept
{
    RtpHeader header;
    const std::span<const std::byte> data = bytes();

    // Byte 0: V/P/X/CC
    std::uint8_t b0 = std::to_integer<std::uint8_t>(data[0]);
    header.version = (b0 >> 6) & 0x03;
    header.padding = (b0 >> 5) & 0x01;
    header.extension = (b0 >> 4) & 0x01;
    header.csrc_count = b0 & 0x0F;

    // Byte 1: M/PT
    std::uint8_t b1 = std::to_integer<std::uint8_t>(data[1]);
    header.marker = (b1 >> 7) & 0x01;
    header.payload_type = b1 & 0x7F;

    // Fixed 16/32-bit fields.
    read_u16_be(data, 2, header.sequence);
    read_u32_be(data, 4, header.timestamp);
    read_u32_be(data, 8, header.ssrc);
    
    // Validate and parse optional CSRC list.
    std::size_t extra_size_requested = kBaseHeaderSize + 4 * header.csrc_count;
    if ( data.size() < extra_size_requested ) {
        fail(PacketError::malformed_packet);
        return {};
    }

    for ( std::size_t i = 0 ; i < header.csrc_count ; i++ ) {
        std::uint32_t csrc = 0;
        read_u32_be(data, 12 + (4*i), csrc);
        header.csrc.push_back(csrc);
    }

    clear_error();
    return header;
}

std::size_t PacketView::header_size() const noexcept
{
    const std::span<const std::byte> data = bytes();
    const std::uint8_t csrc_count = std::to_integer<std::uint8_t>(data[0]) & 0x0F;
    const std::size_t size = kBaseHeaderSize + 4 * csrc_count;
    clear_error();
    return size;
}

std::span<const std::byte> PacketView::payload() const noexcept
{
    // Payload starts right after fixed header + CSRC list.
    const std::span<const std::byte> data = bytes();
    const std::size_t start = header_size();
    const std::size_t end = data.size();

    if (last_error_ != PacketError::none) return {};
    if ( start > end ) {
        fail(PacketError::malformed_packet);
        return {};
    }

    clear_error();
    return data.subspan(start, end - start);
}

bool PacketView::set_header(const RtpHeader& header) noexcept
{
    if (!ensure_mutable()) return false;

    // Validate user-supplied fields.
    if (header.version > 3) return fail(PacketError::invalid_version);
    if (header.csrc_count > 15) return fail(PacketError::invalid_csrc_count);
    if (header.payload_type > 127) return fail(PacketError::invalid_payload_type);
    if (header.csrc.size() != header.csrc_count) return fail(PacketError::csrc_size_mismatch);

    // Ensure destination buffer can hold fixed header + CSRC list.
    const std::size_t size_requested = kBaseHeaderSize + 4 * header.csrc_count;
    if ( buf_.size() < size_requested ) return fail(PacketError::buffer_too_small);

    // Compose first two RTP header bytes.
    std::uint8_t b0 = 
        (( header.version   << 6 ) & 0xC0) |
        (( header.padding   << 5 ) & 0x20) |
        (( header.extension << 4 ) & 0x10) |
        (( header.csrc_count     ) & 0x0F);
    
    std::uint8_t b1 = 
        (( header.marker        << 7 ) & 0x80) |
        (( header.payload_type       ) & 0x7F);

    buf_[0] = static_cast<std::byte>(b0);
    buf_[1] = static_cast<std::byte>(b1);

    // Write fixed numeric fields.
    if (!write_u16_be(buf_, 2, header.sequence)) return fail(PacketError::buffer_too_small);
    if (!write_u32_be(buf_, 4, header.timestamp)) return fail(PacketError::buffer_too_small);
    if (!write_u32_be(buf_, 8, header.ssrc)) return fail(PacketError::buffer_too_small);

    // Write CSRC list.
    for (std::size_t i = 0 ; i < header.csrc_count ; i++ ) {
        if (!write_u32_be(buf_, 12 + 4 * i, header.csrc[i])) return fail(PacketError::buffer_too_small);
    }

    clear_error();
    return true;
}

bool PacketView::set_payload(std::span<const std::byte> data) noexcept
{
    if (!ensure_mutable()) return false;

    // Payload is written right after current RTP header.
    const std::size_t head_size = header_size();
    if (last_error_ != PacketError::none) return false;
    if ( buf_.size() < (head_size + data.size()) ) return fail(PacketError::buffer_too_small);

    // Copy payload bytes in-place.
    std::copy(data.begin(), data.end(), buf_.begin() + head_size);
    clear_error();
    return true;
}

} // namespace vms::rtp
