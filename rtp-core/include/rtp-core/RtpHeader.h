/*
 * Copyright (C) 2026 Manuel Virgilio <real_virgil@yahoo.it>
 *
 * This file is part of vms-rtp.
 * Licensed under the GNU Lesser General Public License v2.1.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <cstdint>
#include <vector>

namespace vms::rtp {

struct RtpHeader {
    std::uint8_t  version = 2;
    bool          padding = false;
    bool          extension = false;
    std::uint8_t  csrc_count = 0;
    bool          marker = false;
    std::uint8_t  payload_type;
    std::uint16_t sequence = 0;
    std::uint32_t timestamp = 0;
    std::uint32_t ssrc = 0;
    std::vector<std::uint32_t> csrc;
};

} // namespace vms::rtp
