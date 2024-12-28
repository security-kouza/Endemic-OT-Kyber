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

#include <indcpa.h>
#include "EndemicOT/Endemic_OT_C.h"

#include <rng.h>

#include "EndemicOT/OTTools.h"

void gen_receiver_message(NewKyberOTRecver* recver, EndemicOTReceiverMsg* pks) {
    uint8_t pk[PKlength];
    uint8_t h[PKlength];
    uint8_t seed[KYBER_SYMBYTES];


    //get pk, sk
    indcpa_keypair(pk, recver->secretKey);

    // sample random public key for the one we dont want.
    randombytes(seed, 32);
    randomPK(pks->keys[1 ^ recver->b], seed, &pk[KYBER_POLYVECBYTES]);

    //compute H(r_{not b})
    pkHash(h, pks->keys[1 ^ recver->b], &pk[KYBER_POLYVECBYTES]);

    //set r_b=pk-H(r_{not b})
    pkMinus(pks->keys[recver->b], pk, h);
}

void gen_sender_message(
    EndemicOTSenderMsg* ctxt,
    const NewKyberOTPtxt* ptxt,
    const EndemicOTReceiverMsg* recvPks
) {
    unsigned char h[PKlength];
    unsigned char pk[PKlength];
    unsigned char coins[Coinslength];
    //compute ct_i=Enc(pk_i, sot_i) and sample coins
    //random coins
    randombytes(coins, Coinslength);
    //compute pk0
    pkHash(h, recvPks->keys[1], recvPks->keys[0] + KYBER_POLYVECBYTES);
    //pk_0=r_0+h(r_1)
    pkPlus(pk, recvPks->keys[0], h);
    //enc
    indcpa_enc(ctxt->sm[0], ptxt->sot[0], pk, coins);
    //random coins
    randombytes(coins, Coinslength);
    //compute pk1
    pkHash(h, recvPks->keys[0], recvPks->keys[0] + KYBER_POLYVECBYTES);
    //pk_1=r_1+h(r_0)
    pkPlus(pk, recvPks->keys[1], h);
    //enc
    indcpa_enc(ctxt->sm[1], ptxt->sot[1], pk, coins);
}

void decrypt_received_data(NewKyberOTRecver* recver, const EndemicOTSenderMsg* ctxt) {
    indcpa_dec(recver->rot, ctxt->sm[recver->b], recver->secretKey);
}
