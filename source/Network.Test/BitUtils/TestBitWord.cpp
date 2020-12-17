// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitSpan.h>

#include <Network/CoreIncludes.h>
#include <gtest/gtest.h>

namespace dud
{

static_assert(sizeof(BitWordType) == sizeof(u64));

TEST(bitword_fixture, danglingMaskFromBitCount_validateCalculations) {
	using namespace bitword;
	{
		constexpr BitWordType mask = bitword::getDanglingPart(0);
		ASSERT_EQ(mask, BitWordType{ 0u });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(1);
		ASSERT_EQ(mask, BitWordType{ 1u });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(2);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(3);
		ASSERT_EQ(mask, BitWordType{ 0b111 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(4);
		ASSERT_EQ(mask, BitWordType{ 0xF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(6);
		ASSERT_EQ(mask, BitWordType{ 0x3f });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(8);
		ASSERT_EQ(mask, BitWordType{ 0xFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(32);
		ASSERT_EQ(mask, BitWordType{ 0xFFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(33);
		ASSERT_EQ(mask, BitWordType{ 0x1FFFFFFFF });
	}

	{
		constexpr BitWordType mask = bitword::getDanglingPart(63);
		ASSERT_EQ(mask, BitWordType{ 0x7FFFFFFFFFFFFFFF });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(64);
		ASSERT_EQ(mask, BitWordType{ 0 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(65);
		ASSERT_EQ(mask, BitWordType{ 1 });
	}
	{
		constexpr BitWordType mask = bitword::getDanglingPart(130);
		ASSERT_EQ(mask, BitWordType{ 0b11 });
	}
}


TEST(bitword_fixture, foreachSetBit_invokedWithCorrectBitIndex)
{
	bool output[NumBitsInWord] = {};
	bool expected[NumBitsInWord] = {};

	BitWordType data = { 0 };
	BitWordType one = 1;

	expected[1] = expected[5] = expected[6] = expected[11] = expected[63] = true;

	for (uint i = 0; i < NumBitsInWord; ++i)
		if (expected[i])
			data |= (one << i);

	bitword::foreachOne(data, [&output](u32 bitIdx) {
		output[bitIdx] = true;
		}
	);

	for (uint i = 0; i < NumBitsInWord; ++i)
		ASSERT_EQ(output[i], expected[i]);
}

TEST(bitword_fixture, foreachSetBit_tresholds)
{
	BitWordType one = 1;

	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		const u32 ExpectedBitIndex = 1;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		const u32 ExpectedBitIndex = 31;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		const u32 ExpectedBitIndex = 32;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
}

TEST(bitword_fixture, foreachSetBit_withReportedIndexOffset)
{
	BitWordType one = 1;
	const u32 IndexOffset = 17;
	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 14;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63 - IndexOffset;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 64 - IndexOffset;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 55;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
}

TEST(bitword_fixture, foreachSetBit_withReportedIndexOffsetCanBeReallyBig)
{
	BitWordType one = 1;
	const u32 IndexOffset = 8181818;
	{
		const u32 ExpectedBitIndex = 0;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 14;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 33;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 55;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
	{
		const u32 ExpectedBitIndex = 63;
		BitWordType data = one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex + IndexOffset); }, IndexOffset);
	}
}

TEST(bitword_fixture, setbit13)
{
	BitWordType value{};

	bitword::setBit(value, 13);

	ASSERT_TRUE(bitword::getBit(value, 13));
}

TEST(bitword_fixture, setAndGet_testAllValidVariations)
{
	BitWordType one = { 1 };
	for (uint i = 0; i < NumBitsInWord; ++i)
	{
		const BitWordType expected = one << i;
		BitWordType value = 0;

		bitword::setBit(value, i);
		ASSERT_TRUE(bitword::getBit(value, i));

		ASSERT_EQ(expected, value);
	}
}

TEST(bitword_fixture, modifyBit_sameAsSetAndClear)
{
	for (uint i = 0; i < NumBitsInWord; ++i)
	{
		{
			BitWordType bitsFromSet, modifiedSet;
			modifiedSet = bitsFromSet = 0x1818181818181818;
			bitword::setBit(bitsFromSet, i);
			bitword::modifyBit(modifiedSet, i, true);

			ASSERT_EQ(bitsFromSet, modifiedSet);
		}
		{
			BitWordType bitsFromCleared, modifiedCleared;
			modifiedCleared = bitsFromCleared = 0xF7F7f7f7f7f7f7f7;
			bitword::clearBit(bitsFromCleared, i);
			bitword::modifyBit(modifiedCleared, i, false);

			ASSERT_EQ(bitsFromCleared, modifiedCleared);
		}
	}
}

#pragma warning( push )
#pragma warning( disable : 4293 ) // warning C4293: '<<': shift count negative or too big, undefined behavior

TEST(bitword_fixture, foreachSetBit_weirdCastBehavior_andUB)
{
	s32 s32_one = 1;

	{
		const u32 ExpectedBitIndex = 3;
		BitWordType data = s32_one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex % 32); });
	}
	{
		const u32 ExpectedBitIndex = 30;
		BitWordType data = s32_one << ExpectedBitIndex;
		bitword::foreachOne(data, [&](u32 bit) { ASSERT_EQ(bit, ExpectedBitIndex); });
	}
	{
		// I think this is a defined behavior
		const u32 ExpectedBitIndex = 31;
		BitWordType data = s32_one << ExpectedBitIndex;
		BitWordType casted = 0xFFFFFFFF80000000;
		ASSERT_EQ(data, casted);
	}
}
#pragma warning( pop )

namespace
{
	BitWordType generateMask(u32 firstBit, u32 lastBit)
	{
		BitWordType output{};
		const BitWordType one = 1u;

		for (uint i = firstBit; i < lastBit; ++i)
		{
			output |= (one << i);
		}

		return output;
	}
}

TEST(bitword_fixture, createMask_canGenerateZeroBasedMasks)
{
	for (uint i = 1; i < NumBitsInWord; ++i)
	{
		const BitWordType created = bitword::createMask(0, i);
		const BitWordType expected = generateMask(0, i);
		ASSERT_EQ(created, expected);
	}
}

TEST(bitword_fixture, createMask_canGenerateOffsetBasedMasks)
{
	u32 firstIdx = 4;
	for (uint i = firstIdx + 1; i < NumBitsInWord; ++i)
	{
		const BitWordType created = bitword::createMask(firstIdx, i);
		const BitWordType expected = generateMask(firstIdx, i);
		ASSERT_EQ(created, expected);
	}

	firstIdx = 11;
	for (uint i = firstIdx + 1; i < NumBitsInWord; ++i)
	{
		const BitWordType created = bitword::createMask(firstIdx, i);
		const BitWordType expected = generateMask(firstIdx, i);
		ASSERT_EQ(created, expected);
	}

	firstIdx = 31;
	for (uint i = firstIdx + 1; i < NumBitsInWord; ++i)
	{
		const BitWordType created = bitword::createMask(firstIdx, i);
		const BitWordType expected = generateMask(firstIdx, i);
		ASSERT_EQ(created, expected);
	}

	firstIdx = 45;
	for (uint i = firstIdx + 1; i < NumBitsInWord; ++i)
	{
		const BitWordType created = bitword::createMask(firstIdx, i);
		const BitWordType expected = generateMask(firstIdx, i);
		ASSERT_EQ(created, expected);
	}
}

}