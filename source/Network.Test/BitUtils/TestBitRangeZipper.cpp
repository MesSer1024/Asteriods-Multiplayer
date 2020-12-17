// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitSpan.h>

#include <Network/CoreIncludes.h>
#include <Network/Core/Meta.h>
#include <gtest/gtest.h>

namespace dud
{

class BitRangeZipperFixture : public testing::Test {
public:
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}
};

static_assert(sizeof(BitWordType) == 8);

TEST_F(BitRangeZipperFixture, foreachWord_countInvocations)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * NumBitsInWord;

	BitWordType _lhs[NumWords];
	BitWordType _rhs[NumWords];

	BitRangeZipper zipper(_lhs, _rhs, BitCount);

	u32 wordCounter = 0;
	zipper.foreachWord([&wordCounter](auto a, auto b) { wordCounter++; });

	ASSERT_EQ(wordCounter, NumWords);
}

TEST_F(BitRangeZipperFixture, foreachWord_countInvocationsWithDanglingBits)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * NumBitsInWord - 17;

	BitWordType _lhs[NumWords];
	BitWordType _rhs[NumWords];

	BitRangeZipper zipper(_lhs, _rhs, BitCount);

	u32 wordCounter = 0;
	zipper.foreachWord([&wordCounter](auto a, auto b) { wordCounter++; });

	ASSERT_EQ(wordCounter, NumWords);
}

TEST_F(BitRangeZipperFixture, foreachWord_paramsProvidedInLinearOrder)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * NumBitsInWord - 17;

	BitWordType inputA[NumWords];
	BitWordType inputB[NumWords];
	BitWordType outputA[NumWords];
	BitWordType outputB[NumWords];
	BitRangeZipper zipper(inputA, inputB, BitCount);

	meta::iota_container(inputA, 0);
	meta::iota_container(inputB, 1000);

	uint it = 0;
	zipper.foreachWord([&it, &outputA, &outputB](auto a, auto b) { outputA[it] = a; outputB[it] = b; it++; });

	ASSERT_EQ(it, NumWords);

	for (uint i = 0; i < NumWords; ++i)
	{
		ASSERT_EQ(outputA[i], inputA[i]);
		ASSERT_EQ(outputB[i], inputB[i]);
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_valuesCanBeReferenced)
{
	const u32 NumWords = 10;
	const u32 BitCount = NumWords * NumBitsInWord - 13;

	BitWordType lhs[NumWords];
	BitWordType rhs[NumWords];
	BitRangeZipper zipper(lhs, rhs, BitCount);

	uint it = 0;
	zipper.foreachWord([&](auto& a, auto& b) { ++it; a = it; b = it + 1000;  });

	ASSERT_EQ(it, NumWords);

	for (uint i = 0; i < NumWords; ++i)
	{
		ASSERT_EQ(lhs[i], i + 1);
		ASSERT_EQ(rhs[i], i + 1001);
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_validateZeroSizeBehavior)
{
	BitRangeZipper zipper(nullptr, nullptr, 0);

	u32 it = 0;
	zipper.foreachWord([&it](auto a, auto b) { ++it; });
	ASSERT_EQ(it, 0u);
}

TEST_F(BitRangeZipperFixture, foreachWord_validateZeroWordSizeAndDanglingBits)
{
	const BitWordType Default = 0xbebebebebe;
	BitWordType outputA[NumBitsInWord];
	BitWordType outputB[NumBitsInWord];

	meta::fill_container(outputA, Default);
	meta::fill_container(outputB, Default);

	uint it = 0;
	for (uint i = 0; i < NumBitsInWord; ++i)
	{
		BitWordType lhs[1] = { bitword::Ones };
		BitWordType rhs[1] = { bitword::Ones };
		
		const u32 BitCount = (i + 1);

		BitRangeZipper zipper(lhs, rhs, BitCount);

		zipper.foreachWord([&it, &outputA, &outputB](auto a, auto b) {
			outputA[it] = a;
			outputB[it] = b;
			it++;
		});

		ASSERT_EQ(it, i + 1);
	}

	ASSERT_EQ(it, NumBitsInWord);
	for (uint i = 0; i < it; ++i)
	{
		const u32 BitCount = (i + 1);
		if (BitCount == NumBitsInWord)
		{
			ASSERT_EQ(outputA[i], bitword::Ones);
			ASSERT_EQ(outputB[i], bitword::Ones);
		}
		else
		{
			const BitWordType expected = bitword::Ones & bitword::getDanglingPartMask(BitCount);
			ASSERT_EQ(outputA[i], expected);
			ASSERT_EQ(outputB[i], expected);
		}
	}
}

TEST_F(BitRangeZipperFixture, foreachWord_validateTresholdBitCountBehavior)
{
	u32 wordCounter;
	auto eachWordAction = [&wordCounter](auto a, auto b) { ++wordCounter;  };
	BitWordType buffer[20];
	for (uint i = 0; i < 10; ++i)
	{
		{ // 63
			wordCounter = 0;
			const u32 bitCount = NumBitsInWord * i + (NumBitsInWord - 1);

			BitRangeZipper zipper(buffer, buffer, bitCount);
			zipper.foreachWord(eachWordAction);

			ASSERT_EQ(wordCounter, i + 1);
		}
		{ // 64
			wordCounter = 0;
			const u32 bitCount = NumBitsInWord * i + (NumBitsInWord + 0);


			BitRangeZipper zipper(buffer, buffer, bitCount);
			zipper.foreachWord(eachWordAction);

			ASSERT_EQ(wordCounter, i + 1);
		}

		{ // 65
			wordCounter = 0;
			const u32 bitCount = NumBitsInWord * i + (NumBitsInWord + 1);

			BitRangeZipper zipper(buffer, buffer, bitCount);
			zipper.foreachWord(eachWordAction);

			ASSERT_EQ(wordCounter, i + 2);
		}
	}
}

}