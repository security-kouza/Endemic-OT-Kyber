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

#include <stddef.h>

#ifdef DEBUG
#include <string.h>
#include <assert.h>
#endif // DEBUG

#include <params.h>
#include <polyvec.h>
#include <indcpa.h>
#include <symmetric.h>

#include "EndemicOT/OTTools.h"

/**
 * START DEPENDENCIES
 *
 * Functions declared static in the Kyber library
 * MUST be updated when the corresponding functions in Kyber are changed.
 */

// Copied from indcpa.c
/*************************************************
* Name:        pack_pk
*
* Description: Serialize the public key as concatenation of the
*              serialized vector of polynomials pk
*              and the public seed used to generate the matrix A.
*
* Arguments:   uint8_t *r:          pointer to the output serialized public key
*              polyvec *pk:         pointer to the input public-key polyvec
*              const uint8_t *seed: pointer to the input public seed
**************************************************/
static void pack_pk(uint8_t r[KYBER_INDCPA_PUBLICKEYBYTES],
                    polyvec *pk,
                    const uint8_t seed[KYBER_SYMBYTES])
{
    size_t i;
    polyvec_tobytes(r, pk);
    for(i=0;i<KYBER_SYMBYTES;i++)
        r[i+KYBER_POLYVECBYTES] = seed[i];
}
/**
 * END DEPENDENCIES
 */

// [r] = [a] - [b]
static void polyvec_sub(polyvec *r, const polyvec *a, const polyvec *b)
{
    size_t i;
    for(i=0;i<KYBER_K;i++)
        poly_sub(&r->vec[i], &a->vec[i], &b->vec[i]);
}

void pkHash(uint8_t* output, const uint8_t* pk, const uint8_t* pkSeed) {
    uint8_t rawHashOutput[KYBER_SYMBYTES];
    hash_h(&rawHashOutput[0], pk, KYBER_POLYVECBYTES);
    randomPK(output, &rawHashOutput[0], pkSeed);
}

void pkMinus(uint8_t* pk, const uint8_t* pk1, const uint8_t* pk2) {
#ifdef DEBUG
    // with the same seed
    assert(0 == memcmp(pk1 + KYBER_POLYVECBYTES, pk2 + KYBER_POLYVECBYTES, KYBER_SYMBYTES));
#endif // DEBUG
    polyvec pkpv1, pkpv2;
    const uint8_t* SEED = pk1 + KYBER_POLYVECBYTES;
    polyvec_frombytes(&pkpv1, pk1);
    polyvec_frombytes(&pkpv2, pk2);
    polyvec_sub(&pkpv1, &pkpv1, &pkpv2);
    polyvec_reduce(&pkpv1);
    pack_pk(pk, &pkpv1, SEED);
}

void pkPlus(uint8_t* pk, const uint8_t* pk1, const uint8_t* pk2) {
#ifdef DEBUG
    // with the same seed
    assert(0 == memcmp(pk1 + KYBER_POLYVECBYTES, pk2 + KYBER_POLYVECBYTES, KYBER_SYMBYTES));
#endif // DEBUG
    polyvec pkpv1, pkpv2;
    const uint8_t* SEED = pk1 + KYBER_POLYVECBYTES;
    polyvec_frombytes(&pkpv1, pk1);
    polyvec_frombytes(&pkpv2, pk2);
    polyvec_add(&pkpv1, &pkpv1, &pkpv2);
    polyvec_reduce(&pkpv1);
    pack_pk(pk, &pkpv1, SEED);
}

void randomPK(uint8_t* pk, const uint8_t* seed1, const uint8_t* seed2) {
    polyvec a[KYBER_K];
    gen_matrix(a, seed1, 0);
    pack_pk(pk, &a[0], seed2);
}
