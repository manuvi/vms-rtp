/*
 * Copyright (C) 2026 Manuel Virgilio <real_virgil@yahoo.it>
 *
 * This file is part of vms-rtp.
 * Licensed under the GNU Lesser General Public License v2.1.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <limits>
#include <random>
#include <type_traits>

namespace vms::rtp {

class Random {
public:
    Random()
        : eng_(make_seed())
    {}

    template <typename T>
    T next() {
        static_assert(std::is_integral_v<T>, "T must be integral ttpe");

        std::uniform_int_distribution<T> dist(
            std::numeric_limits<T>::min(),
            std::numeric_limits<T>::max()
        );

        return dist(eng_);
    }

private:
    using engine_t = std::mt19937;

    static engine_t make_seed() {
        std::random_device rd;

        std::seed_seq seq {
            rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()
        };

        return engine_t(seq);
    }

    engine_t eng_;
};

} // namespace vms::rtp