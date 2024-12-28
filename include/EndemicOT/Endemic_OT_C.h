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

*******************************************************************************

This file contains code derived from LibOTe, which is distributed under the MIT License.
For more details, visit https://github.com/osu-crypto/libOTe.

Copyright (C) 2016  Peter Rindal
Copyright (C) 2022  Visa

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.
*/

#ifndef LIBOTE_NEWKYBEROT_H
#define LIBOTE_NEWKYBEROT_H

#include <stdint.h>
//length in bytes/unsigned chars
#include <params.h>

#define PKlength KYBER_INDCPA_PUBLICKEYBYTES
#define SKlength KYBER_INDCPA_SECRETKEYBYTES
#define CTlength (KYBER_POLYVECBYTES+KYBER_POLYBYTES)
#define OTlength KYBER_INDCPA_MSGBYTES
#define Coinslength (32)

typedef struct
{
    // sender message
    uint8_t sm[2][CTlength];
} EndemicOTSenderMsg;

typedef __attribute__ ((aligned(32))) struct
{
    // sender chosen strings
    uint8_t sot[2][OTlength];
} NewKyberOTPtxt;

//receiver message 1 in the Kyber OT protocol.
typedef struct
{
    // receiver message
    uint8_t keys[2][PKlength];
} EndemicOTReceiverMsg;

typedef struct
{
    // receiver secret randomness
    uint8_t secretKey[SKlength];

    // receiver learned strings
    uint8_t rot[OTlength];

    // receiver choice bit
    uint8_t b;
} NewKyberOTRecver;

void gen_receiver_message(NewKyberOTRecver* recver, EndemicOTReceiverMsg* pks);
void gen_sender_message(EndemicOTSenderMsg* ctxt, const NewKyberOTPtxt* ptxt, const EndemicOTReceiverMsg* recvPks);
void decrypt_received_data(NewKyberOTRecver* recver, const EndemicOTSenderMsg* ctxt);

#endif // LIBOTE_NEWKYBEROT_H
