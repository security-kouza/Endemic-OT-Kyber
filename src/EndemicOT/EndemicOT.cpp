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

#include <ostream>
#include <iomanip>
#include <cstring>

#ifdef DEBUG
#include <cassert>
#endif

#include "EndemicOT/EndemicOT.hpp"

namespace ATLab::EndemicOT {

    // TODO:
    // memcpy can be optimized out by changing OT.c to cpp and then changing const references to move semantics;

    Receiver::Receiver(const bool choiceBit):
        _stage{Stage::INIT},
        _ot{},
        _pkBuff{}
    {
        _ot.b = choiceBit;
        gen_receiver_message(&_ot, &_pkBuff);
    }

    Receiver::Receiver(Receiver&& movedReceiver) noexcept
        : _stage{movedReceiver._stage},
        _ot{movedReceiver._ot},
        _pkBuff{movedReceiver._pkBuff}
    {
        movedReceiver._stage = Stage::END;
    }

    ReceiverMsg Receiver::get_receiver_msg() const {
        return _pkBuff;
    }

    DataBlock Receiver::decrypt_chosen(const EndemicOTSenderMsg& ctxts) {
    #ifdef DEBUG
        assert(_stage == Stage::INIT);
    #endif

        decrypt_received_data(&_ot, &ctxts);
        _stage = Stage::END;

        DataBlock block;
        std::copy(std::begin(_ot.rot), std::end(_ot.rot), block.begin());
        return block;
    }

    SenderMsg Sender::encrypt_with(const ReceiverMsg& pkPair) const {
        NewKyberOTPtxt ptxt;
        EndemicOTSenderMsg ctxt;
        constexpr size_t DATA_BYTES {sizeof(Data)};

        memcpy(ptxt.sot[0], &_data0, DATA_BYTES);

        memcpy(ptxt.sot[1], &_data1, DATA_BYTES);

        //get senders message, secret coins and ot strings
        gen_sender_message(&ctxt, &ptxt, &pkPair);

        return ctxt;
    }

    void batch_send(
        ATLab::Socket& socket,
        const __m128i* const data0,
        const __m128i* const data1,
        const size_t length
    ) {
        Sender::Data d0, d1;
        for (size_t i{0}; i != length; ++i) {
            // resetting the last 128 bits is not necessary since `recv` does not use those uninitialized bits
            memcpy(&d0, &data0[i], sizeof(__m128i));
            memcpy(&d1, &data1[i], sizeof(__m128i));
            Sender sender(d0, d1);
            ReceiverMsg rMsg;
            socket.read(&rMsg, sizeof(rMsg), "EOT sender: reading receiver's message.");
            auto sMsg{sender.encrypt_with(rMsg)};
            socket.write(&sMsg, sizeof(sMsg), "EOT sender: sending encrypted data.");
        }
    }

    void batch_receive(
        ATLab::Socket& socket,
        __m128i* const data,
        const bool* const choices,
        const size_t length
    ) {
        for (size_t i{0}; i != length; ++i) {
            Receiver receiver(choices[i]);
            auto rMsg{receiver.get_receiver_msg()};
            socket.write(&rMsg, sizeof(rMsg), "EOT receiver: sending receiver's message.");
            SenderMsg sMsg;
            socket.read(&sMsg, sizeof(sMsg), "EOT receiver: receiving sender message to decrypt");
            auto decryptedData{receiver.decrypt_chosen(sMsg)};
            memcpy(&data[i], &decryptedData, sizeof(__m128i));
        }
    }
}
