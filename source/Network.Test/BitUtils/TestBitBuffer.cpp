// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitBuffer.h>

#include <gtest/gtest.h>

namespace dud
{

class BitBufferFixture : public testing::Test {
public:
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}
};

TEST_F(BitBufferFixture, ctor_ones_dataIsAllOnes)
{
	const u32 NumWords = 10;
	BitBuffer buffer(BitBuffer::OneInit, NumWords * NumBitsInWord - 5);

	u32 counter = 0;
	for (auto&& word : buffer)
	{
		ASSERT_EQ(word, bitword::Ones);
		counter++;
	}
	ASSERT_EQ(counter, NumWords);
}

TEST_F(BitBufferFixture, ctor_zero_dataIsZeroed)
{
	const u32 NumWords = 10;
	BitBuffer buffer(BitBuffer::OneInit, NumWords * NumBitsInWord - 5);

	u32 counter = 0;
	for (auto&& word : buffer)
	{
		ASSERT_EQ(word, bitword::Ones);
		counter++;
	}
	ASSERT_EQ(counter, NumWords);
}

TEST_F(BitBufferFixture, size_reflectsAllocatedWordCount)
{
	auto bitToNumBytes = [](u32 numBits) { return bitword::getNumBytesRequiredToRepresentWordBasedBitBuffer(numBits); };

	{
		BitBuffer buffer(BitBuffer::OneInit, 0);
		ASSERT_EQ(buffer.size(), 0u);
	}
	{
		const u32 numBits = 1;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 8;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 31;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 32;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 63;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 64;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 8u);
	}
	{
		const u32 numBits = 65;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 16u);
	}
	{
		const u32 numBits = 127;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 16u);
	}
	{
		const u32 numBits = 128;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 16u);
	}
	{
		const u32 numBits = 129;
		BitBuffer buffer(BitBuffer::OneInit, numBits);
		ASSERT_EQ(buffer.size(), 24u);
	}
}

}