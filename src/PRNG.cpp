/*
This file is part of EOTKyber of Abe-Tibouchi Laboratory, Kyoto University
Copyright (C) 2023-2024  Kyoto University
Copyright (C) 2023-2024  Peihao Li <li.peihao.62s@st.kyoto-u.ac.jp>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PRNG.hpp"

extern "C" {
#include <rng.h>
}

namespace ATLab {
    PRNG_Kyber& PRNG_Kyber::get_PRNG_Kyber(std::array<uint8_t, 48> seed) {
        static PRNG_Kyber KyberInstance; // state stored in rng.c
        static bool firstInvocation {true};
        if (!firstInvocation) {
            return KyberInstance;
        }

        randombytes_init(seed.data(), nullptr, 0); // since `security_strength` is ignored. Bad design :(
        firstInvocation = false;
        return KyberInstance;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    PRNG_Kyber::result_type PRNG_Kyber::operator()() {
        std::array<uint8_t, 16> buf {};
        randombytes(buf.data(), buf.size());
        return *reinterpret_cast<result_type*>(buf.data());
    }

    std::vector<bool> random_bool_vector(const size_t len) {
        constexpr size_t BLOCK_SIZE {128};

        std::vector<bool> res(len);

        auto copy_bits_reverse {[](
            std::vector<bool>::iterator it,
            __uint128_t block,
            const size_t copyLength
        ) {
#ifdef DEBUG
            assert(copyLength <= BLOCK_SIZE);
#endif // DEBUG
            for (size_t j {0}; j != copyLength; ++j) {
                *it = block & 1;
                ++it;
                block >>= 1;
            }
        }};

        auto itBit {res.begin()};
        auto PRNG {PRNG_Kyber::get_PRNG_Kyber()};

        for (size_t i {0}; i != len / BLOCK_SIZE; ++i) {
            const auto randBlock {PRNG()};
            copy_bits_reverse(itBit, randBlock, BLOCK_SIZE);
            itBit += BLOCK_SIZE;
        }
        if (const size_t remainingBitsCnt {len % BLOCK_SIZE}) {
            const auto randBlock{PRNG()};
            copy_bits_reverse(itBit, randBlock, remainingBitsCnt);
            // itBit not used anymore, so no need to move itBit
        }

        return res;
    }
}
