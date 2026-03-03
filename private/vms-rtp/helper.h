/*
 * Copyright (C) 2026 Manuel Virgilio <real_virgil@yahoo.it>
 *
 * This file is part of vms-rtp.
 * Licensed under the GNU Lesser General Public License v2.1.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <bit>
#include <cstdint>
#include <cstring>
#include <span>

#include <byteswap.h>

namespace vms::rtp {

template <class T>
constexpr T byteswap(T v) {
    if constexpr (sizeof(T) == 2) return static_cast<T>(bswap_16(v));
    if constexpr (sizeof(T) == 4) return static_cast<T>(bswap_32(v));
    if constexpr (sizeof(T) == 8) return static_cast<T>(bswap_64(v));
    return v;
}

template <class T>
constexpr T host_to_be(T v)
{
    if constexpr (std::endian::native == std::endian::little) return byteswap(v);
    else return v;
}

template <class T>
constexpr T be_to_host(T v)
{
    if constexpr (std::endian::native == std::endian::little) return byteswap(v);
    else return v;
}

inline bool read_u16_be(std::span<const std::byte> b, std::size_t offset, std::uint16_t& out) noexcept
{ 
    if (offset +  sizeof(std::uint16_t) > b.size()) return false;

    std::uint16_t data;
    std::memcpy(&data, b.data() + offset, sizeof(data));
    out = be_to_host(data);
    return true;
}

inline bool read_u32_be(std::span<const std::byte> b, std::size_t offset, std::uint32_t& out) noexcept
{
    if (offset +  sizeof(std::uint32_t) > b.size()) return false;

    std::uint32_t data;
    std::memcpy(&data, b.data() + offset, sizeof(data));
    out = be_to_host(data);
    return true;
}

inline bool write_u16_be(std::span<std::byte> b, std::size_t offset, std::uint16_t v) noexcept
{
    if (offset +  sizeof(std::uint16_t) > b.size()) return false;

    std::uint16_t data = host_to_be(v);
    std::memcpy(b.data() + offset, &data, sizeof(data));
    return true;
}

inline bool write_u32_be(std::span<std::byte> b, std::size_t offset, std::uint32_t v) noexcept
{
    if (offset +  sizeof(std::uint32_t) > b.size()) return false;

    std::uint32_t data = host_to_be(v);
    std::memcpy(b.data() + offset, &data, sizeof(data));
    return true;
}

}
