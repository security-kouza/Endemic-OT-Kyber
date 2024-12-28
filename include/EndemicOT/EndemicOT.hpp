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

#ifndef ENDEMIC_OT_USING_KYBER
#define ENDEMIC_OT_USING_KYBER

#include <cstdint>
#include <array>
#include <emmintrin.h>

#include <socket.hpp>

namespace ATLab::EndemicOT {

    /** For Usage:
     * Please refer to the codes of `batch_send` & `batch_receive`
     */

extern "C" {
#include "Endemic_OT_C.h"
}

    using DataBlock = __attribute__((aligned(32))) std::array<uint8_t, 32>;

    using ReceiverMsg = EndemicOTReceiverMsg;
    using SenderMsg = EndemicOTSenderMsg;

    class Receiver {
        enum Stage { INIT, END };
        Stage _stage;
        NewKyberOTRecver _ot;
        EndemicOTReceiverMsg _pkBuff;

    public:
        Receiver() = delete;
        ~Receiver() = default;
        Receiver(Receiver&) = delete;
        Receiver(Receiver&&) noexcept;

        // This ctor is "non-deterministic"
        explicit Receiver(bool choiceBit);

        ReceiverMsg get_receiver_msg() const;
        DataBlock decrypt_chosen(const EndemicOTSenderMsg& ctxts);
    };

    class Sender {
    public:
        using Data = DataBlock;
    private:
        // TODO: Implement stage
        enum Stage { INIT, END };
        Data _data0, _data1;

    public:
        Sender() = delete;
        ~Sender() = default;
        Sender(const Sender&) = default;
        Sender(Sender&&) = default;
        Sender(const Data& data0, const Data& data1): _data0{data0}, _data1{data1} {};

        SenderMsg encrypt_with(const ReceiverMsg&) const;
    };

    void batch_send(
        ATLab::Socket&  socket,
        const __m128i*  data0,
        const __m128i*  data1,
        size_t          length
    );

    void batch_receive(
        ATLab::Socket&  socket,
        __m128i*        data,
        const bool*     choices,
        size_t          length
    );
}

#endif
