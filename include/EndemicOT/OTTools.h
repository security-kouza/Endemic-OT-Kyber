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

#ifndef LIBOTE_OTTOOLS_H
#define LIBOTE_OTTOOLS_H

#include <stdint.h>

//compute H(pk) and store it in output
void pkHash(uint8_t* output, const uint8_t* pk, const uint8_t* pkSeed);

// [pk] <- [pk1] - [pk2]
void pkMinus(uint8_t* pk, const uint8_t* pk1, const uint8_t* pk2);

// [pk] <- [pk1] + [pk2]
void pkPlus(uint8_t* pk, const uint8_t* pk1, const uint8_t* pk2);

void randomPK(uint8_t* pk, const uint8_t* seed1, const uint8_t* seed2);

#endif //LIBOTE_OTTOOLS_H
