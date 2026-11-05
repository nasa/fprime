// Copyright 2025 California Institute of Technology
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

/*
    This file contains a CRC32 table generator based on the implementation from
    https://create.stephan-brumme.com/crc32/#sarwate.

    Here is the original software license:

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include <stdint.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    uint32_t polynomial = 0xEDB88320;

    uint32_t Crc32Lookup[4][0x100];

    for (uint32_t i = 0; i < 0x100; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-(uint32_t)(crc & 1) & polynomial);
        }
        Crc32Lookup[0][i] = crc;
    }
    for (uint32_t i = 0; i < 0x100; i++) {
        Crc32Lookup[1][i] = (Crc32Lookup[0][i] >> 8) ^ Crc32Lookup[0][Crc32Lookup[0][i] & 0xFF];
        Crc32Lookup[2][i] = (Crc32Lookup[1][i] >> 8) ^ Crc32Lookup[0][Crc32Lookup[1][i] & 0xFF];
        Crc32Lookup[3][i] = (Crc32Lookup[2][i] >> 8) ^ Crc32Lookup[0][Crc32Lookup[2][i] & 0xFF];
    }

    for (uint32_t k = 0; k < 4; k++) {
        printf("static const U32 crc32_ieee802_3_lookup%d[256] = {", k);
        for (uint32_t i = 0; i < 0x100; i++) {
            if (i % 4 == 0) {
                printf("\n   ");
            }
            printf(" 0x%08X,", Crc32Lookup[k][i]);
        }
        printf("\n};\n");
    }
    return 0;
}
