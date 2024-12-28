/*
This file is part of EOTKyber of Abe-Tibouchi Laboratory, Kyoto University
Copyright © 2023-2024  Kyoto University
Copyright © 2023-2024  Peihao Li <li.peihao.62s@st.kyoto-u.ac.jp>

EOTKyber is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EOTKyber is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ATLab_PRNG_HPP
#define ATLab_PRNG_HPP

#include <vector>
#include <mutex>
#include <iomanip>
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <array>
#include <limits>

namespace ATLab {

    /**
     * Usage:
     *
     */

    // Singleton, since Kyber/rng.c uses a global variable to store the inner state
    class PRNG_Kyber {
    public:
        using result_type = __uint128_t;

    private:
        PRNG_Kyber() = default;

    public:
        static constexpr size_t SEED_LENGTH {48}; // Size of argument `entropy_input` of `randombytes` in Kyber

        // At first invocation, init Kyber rng with seed
        // After
        static PRNG_Kyber& get_PRNG_Kyber(std::array<uint8_t, 48> seed = {0});

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() {
            // constexpr uint64_t ALL_SET {0xFFFFFFFFFFFFFFFFULL};
            // return static_cast<result_type>(ALL_SET) << 64 | ALL_SET;
            return std::numeric_limits<__uint128_t>::max();
        }

        result_type operator()();
    };

    // generate random array using Kyber's PNG
    template <size_t N>
    std::array<bool, N> random_bool_array() {
        std::array<bool, N> res;
        constexpr size_t block128Needed {N / 128 + ((N % 128 != 0) ? 1 : 0)};
        std::array<__uint128_t, block128Needed> buf {};

        auto PRNG {PRNG_Kyber::get_PRNG_Kyber()};
        for (auto& block : buf) {
            block = PRNG();
        }

        auto* pByte {reinterpret_cast<unsigned char*>(buf.data())};
        for (size_t iBit {0}; iBit != N; ++iBit) {
            if (iBit && iBit % 8 == 0) {
                ++pByte;
            }
            res.at(iBit) = *pByte & 1;
            *pByte >>= 1;
        }
        return res;
    }

    // emp compatible
    std::vector<bool> random_bool_vector(size_t length);


}

#endif //ATLab_PRNG_HPP
