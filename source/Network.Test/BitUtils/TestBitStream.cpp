// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitStream.h>
#include <Network/Core/Meta.h>

#include <gtest/gtest.h>

namespace dud
{

class BitStreamFixture : public testing::Test {
public:
protected:
	void SetUp() override {
	}

	void TearDown() override {
	}
};

TEST_F(BitStreamFixture, validate_ctor)
{
	BitStream stream;

	ASSERT_EQ(stream.tell(), 0u);
}

TEST_F(BitStreamFixture, canPopulateFromAndTransferTo)
{
	u8 input[100];
	u8 output[100];

	meta::iota_container(output, 1);
	meta::fill_container(input, 0xFF);

	// validate different
	for (uint i = 0, end = sizeof(input); i < end; ++i)
	{
		ASSERT_NE(input[i], output[i]);
	}

	BitStream stream;
	stream.populateFrom({ input, sizeof(input) });
	stream.transferTo({ output, sizeof(output) });

	// validate same
	for (uint i = 0, end = sizeof(input); i < end; ++i)
	{
		ASSERT_EQ(input[i], output[i]);
	}
}

TEST_F(BitStreamFixture, TransferTo_WithDifferentOutputTresholds)
{
	u8 input[100];
	u8 output[100];

	meta::fill_container(input, 0xFF);

	BitStream stream;
	stream.populateFrom({ input, sizeof(input) });

	{
		meta::iota_container(output, 1);
		const u32 ExpectedBytes = 45;
		u32 transferredBytes = stream.transferTo({ output, ExpectedBytes });

		ASSERT_EQ(transferredBytes, ExpectedBytes);

		for (uint i = 0; i < transferredBytes; ++i)
		{
			ASSERT_EQ(input[i], output[i]);
		}
	}

	{
		meta::iota_container(output, 1);
		const u32 ExpectedBytes = 13;
		u32 transferredBytes = stream.transferTo({ output, ExpectedBytes });

		ASSERT_EQ(transferredBytes, ExpectedBytes);

		for (uint i = 0; i < transferredBytes; ++i)
		{
			ASSERT_EQ(input[i], output[i]);
		}
	}

	{
		meta::iota_container(output, 1);
		const u32 ExpectedBytes = 99;
		u32 transferredBytes = stream.transferTo({ output, ExpectedBytes });

		ASSERT_EQ(transferredBytes, ExpectedBytes);

		for (uint i = 0; i < transferredBytes; ++i)
		{
			ASSERT_EQ(input[i], output[i]);
		}
	}

	{
		meta::iota_container(output, 1);
		const u32 ExpectedBytes = 100;
		u32 transferredBytes = stream.transferTo({ output, ExpectedBytes });

		ASSERT_EQ(transferredBytes, ExpectedBytes);

		for (uint i = 0; i < transferredBytes; ++i)
		{
			ASSERT_EQ(input[i], output[i]);
		}
	}
}

TEST_F(BitStreamFixture, populateFrom_modifiesBitPosition)
{
	u8 input[100];

	BitStream stream;
	stream.populateFrom({ input, 3 });
	ASSERT_EQ(stream.tell(), 24u);
}

TEST_F(BitStreamFixture, seek_movesBitPosition)
{
	BitStream stream;

	for (uint i = 0; i < 1000; i+=7)
	{
		stream.seek(i);
		ASSERT_EQ(stream.tell(), i);
	}

	for (int i = 1000; i >= 0; i -= 7)
	{
		stream.seek(i);
		ASSERT_EQ(stream.tell(), i);
	}
}

TEST_F(BitStreamFixture, readUnsigned_canReadConsecutiveBits)
{
	u8 input[100];
	input[0] = 0xEA;
	input[1] = 0xCA;
	input[2] = 0xFE;
	BitStream stream;

	stream.populateFrom({ input, 10 });
	stream.seek(0);

	u32 parts[6];
	for (uint i = 0; i < 6; ++i)
	{
		parts[i] = stream.readUnsigned(4);
	}
}

TEST_F(BitStreamFixture, readUnsigned_affectedByBitPos)
{
	u8 input[100];
	const u8 BytePattern = 0b11001011;
	input[0] = BytePattern;
	input[1] = BytePattern;
	input[2] = BytePattern;
	input[3] = BytePattern;

	const u32 Pattern = BytePattern << 24 | BytePattern << 16 | BytePattern << 8 | BytePattern;

	BitStream stream;

	stream.populateFrom({ input, 10 });
	stream.seek(0);

	for (uint i = 0; i < 10; ++i)
	{
		stream.seek(i);
		const u32 data = stream.readUnsigned(4);
		const u32 Expected = (Pattern >> i) & 0b1111;

		ASSERT_EQ(data, Expected);
	}
}


}