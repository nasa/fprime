// Copyright 2026 California Institute of Technology
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

#include <Utils/Hash/Hash.hpp>
#include <algorithm>
#include "gtest/gtest.h"

TEST(Crc32Test, testEmptyInput) {
    Utils::HashBuffer hb;
    Utils::Hash::hash("", 0, hb);
    U32 output = hb.asBigEndianU32();
    EXPECT_EQ(output, 0);
}

TEST(Crc32Test, testHelloWorld) {
    Utils::HashBuffer hb;
    U8 input[] = "Hello, World!";
    Utils::Hash::hash(input, sizeof(input) - 1 /* null terminator */, hb);
    U32 output = hb.asBigEndianU32();
    EXPECT_EQ(output, 0xEC4AC3D0);
}

TEST(Crc32Test, testBinary) {
    U8 input[256];
    for (U32 i = 0; i < sizeof(input); i++) {
        input[i] = U8(i);
    }

    Utils::HashBuffer hb;
    Utils::Hash::hash(input, sizeof(input), hb);
    U32 output = hb.asBigEndianU32();
    EXPECT_EQ(output, 0x29058C73);
}

// We need sample data to operate on, how about this?
static const U8 sample_input[] =
    "Public Law 85-568\n"
    "\n"
    "AN ACT\n"
    "\n"
    "To provide for research into problems of flight within and outside the earth's\n"
    "atmosphere, and for other purposes.\n"
    "\n"
    "Be it enacted by the Senate and House of Representatives of the\n"
    "United States of America in Congress assembled,\n"
    "\n"
    "TITLE I - SHORT TITLE, DECLARATION OF POLICY, AND DEFINITIONS\n"
    "\n"
    "SHORT TITLE\n"
    "\n"
    "SEC. 101. This Act may be cited as the \"National Aeronautics and"
    "Space Act of 1958\"."
    "\n"
    "DECLARATION OF POLICY AND PURPOSE\n"
    "\n"
    "SEC. 102. (a) The Congress hereby declares that it is the policy of\n"
    "the United States that activities in space should be devoted to peaceful\n"
    "purposes for the benefit of all mankind.\n"
    "\n"
    "(b) The Congress declares that the general welfare and security of\n"
    "the United States require that adequate provision be made for aeronautical\n"
    "and space activities. The Congress further declares that\n"
    "such activities shall be the responsibility of, and shall be directed by,\n"
    "a civilian agency exercising control over aeronautical and space activities\n"
    "sponsored by the United States, except that activities peculiar\n"
    "to or primarily associated with the development of weapons systems,\n"
    "military operations, or the defense of the United States (including\n"
    "the research and development necessary to make effective provision for\n"
    "the defense of the United States) shall be the responsibility of, and\n"
    "shall be directed by, the Department of Defense; and that determination\n"
    "as to which such agency has responsibility for and direction of\n"
    "any such activity shall be made by the President in conformity with\n"
    "section 201 (e).\n"
    "\n"
    "(c) The aeronautical and space activities of the United States shall\n"
    "be conducted so as to contribute materially to one or more of the\n"
    "following objectives:\n"
    "(1) The expansion of human knowledge of phenomena in the\n"
    "atmosphere and space;\n"
    "(2) The improvement of the usefulness, performance, speed,\n"
    "safety, and efficiency of aeronautical and space vehicles;\n"
    "(3) The development and operation of vehicles capable of\n"
    "carrying instruments, equipment, supplies, and living organisms\n"
    "through space;\n"
    "(4) The establishment of long-range studies of the potential\n"
    "benefits to be gained from, the opportunities for, and the problems\n"
    "involved in the utilization of aeronautical and space activities for\n"
    "peaceful and scientific purposes;\n"
    "(5) The preservation of the role of the United States as a\n"
    "leader in aeronautical and space science and technology and in\n"
    "the application thereof to the conduct of peaceful activities within\n"
    "and outside the atmosphere;\n"
    "(6) The making available to agencies directly concerned with\n"
    "national defense of discoveries that have military value or significance,\n"
    "and the furnishing by such agencies, to the civilian agency\n"
    "established to direct and control nonmilitary aeronautical and\n"
    "space activities, of information as to discoveries which have value\n"
    "or significance to that agency;\n"
    "(7) Cooperation by the United States with other nations and\n"
    "groups of nations in work done pursuant to this Act and in the\n"
    "peaceful application of the results thereof; and\n"
    "(8) The most effective utilization of the scientific and engineer-\n"
    "ing resources of the United States, with close cooperation among\n"
    "all interested agencies of the United States in order to avoid\n"
    "unnecessary duplication of effort, facilities, and equipment.\n"
    "\n"
    "(d) It is the purpose of this Act to carry out and effectuate the\n"
    "policies declared in subsections (a), (b), and (c),\n"
    "\n"
    "DEFINITIONS\n"
    "\n"
    "SEC. 103. As used in this Act--\n"
    "\n"
    "(1) the term \"aeronautical and space activities\" means (A)\n"
    "research into, and the solution of, problems of flight within and\n"
    "outside the earth's atmosphere, (B) the development, construction,\n"
    "testing, and operation for research purposes of aeronautical\n"
    "and space vehicles, and (C) such other activities as may be\n"
    "required for the exploration of space; and\n"
    "\n"
    "(2) the term \"aeronautical and space vehicles\" means air-\n"
    "craft, missiles, satellites, and other space vehicles, manned and\n"
    "unmanned, together with related equipment, devices, components,\n"
    "and parts.\n";

constexpr U32 sample_input_hash = 0x5E6BB4BD;
constexpr U32 sample_input_length = sizeof(sample_input) - 1 /* null terminator */;

TEST(Crc32Test, testLongText) {
    Utils::HashBuffer hb;
    Utils::Hash::hash(sample_input, sample_input_length, hb);
    U32 output = hb.asBigEndianU32();
    EXPECT_EQ(output, sample_input_hash);
}

const U32 varyingSizeVectors[] = {0xB969BE79, 0xB02D9F38, 0x620C7FA4, 0x8091341E, 0x55FE1940, 0x2978FC8E,
                                  0x16C9BBDA, 0x35075533, 0x2D058B95, 0x6EBD4DD1, 0xD6CCCFCA, 0xFF4A3832};

TEST(Crc32Test, testVaryingSizes) {
    Utils::Hash hash;
    for (U32 i = 0; i < FW_NUM_ARRAY_ELEMENTS(varyingSizeVectors); i++) {
        U32 offset = (1 << i) - 1;
        U32 length = std::min(U32(1 << i), U32(sample_input_length - offset));
        hash.init();
        hash.update(sample_input + offset, length);
        U32 output;
        hash.finalize(output);
        EXPECT_EQ(output, varyingSizeVectors[i]);
    }
}

TEST(Crc32Test, testSimpleMultiUpdate) {
    Utils::Hash hash;
    hash.update(sample_input, sizeof(sample_input) / 2);
    hash.update(sample_input + sizeof(sample_input) / 2, sample_input_length - sizeof(sample_input) / 2);
    U32 output;
    hash.finalize(output);
    EXPECT_EQ(output, sample_input_hash);
}

TEST(Crc32Test, testMultiUpdatePartitions) {
    Utils::Hash hash;
    for (U32 i = 0; i <= sample_input_length; i++) {
        hash.init();
        hash.update(sample_input, i);
        hash.update(sample_input + i, sample_input_length - i);
        U32 output;
        hash.finalize(output);
        EXPECT_EQ(output, sample_input_hash);
    }
}

TEST(Crc32Test, testVeryLargeInput) {
    // Pick a size (20 megabytes) that is large enough to use more than 24 bits
    constexpr U32 largeSize = 20 * 1000 * 1000;
    U8* largeInput = static_cast<U8*>(malloc(largeSize));
    FW_ASSERT(largeInput != nullptr);

    // Fill input with arbitrary data
    U32 value = largeSize;
    for (U32 i = 0; i < largeSize; i++) {
        value -= (i * i) ^ i;
        value ^= sample_input[i % sample_input_length];
        largeInput[i] = value & 0xFF;
    }

    // Checksum the whole thing all at once
    Utils::HashBuffer allAtOnce;
    Utils::Hash::hash(largeInput, largeSize, allAtOnce);

    // Then checksum it 13 bytes at a time.
    Utils::Hash hash;
    U32 limit = largeSize - largeSize % 13;
    for (U32 i = 0; i < limit; i += 13) {
        hash.update(largeInput + i, 13);
    }
    hash.update(largeInput + limit, largeSize - limit);
    U32 output;
    hash.finalize(output);

    EXPECT_EQ(output, allAtOnce.asBigEndianU32());

    free(largeInput);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
