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

TEST_F(BitStreamFixture, readWriteWithZeroSize)
{
	BitStream stream;
	stream.writeUnsigned(0xFFFF, 12);
	stream.seek(0);

	ASSERT_EQ(stream.readUnsigned(0), 0u);
	ASSERT_EQ(stream.tell(), 0u);

	stream.writeUnsigned(0xFF, 0u);
	ASSERT_EQ(stream.tell(), 0u);
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

TEST_F(BitStreamFixture, writeUnsigned_affectsBitPosition)
{
	BitStream stream;
	for (uint i = 0; i < 10; ++i)
	{
		stream.writeUnsigned(0xfefe, 4);
		const u32 ExpectedPosition = (i + 1) * 4;
		ASSERT_EQ(stream.tell(), ExpectedPosition);
	}
}

TEST_F(BitStreamFixture, writeUnsigned_transferredDataLooksAsExpected)
{
	u8 output[100];

	BitStream stream;
	stream.writeUnsigned(0xE, 4);
	stream.writeUnsigned(0xF, 4);
	stream.writeUnsigned(0xA, 4);
	stream.writeUnsigned(0xC, 4);
	stream.writeUnsigned(0xA, 4);
	stream.writeUnsigned(0xE, 4);

	stream.transferTo({ output, 4 });
	u64 data{};
	memcpy(&data, output, 4);

	ASSERT_EQ(data, 0xEACAFE);
}

TEST_F(BitStreamFixture, writeUnsigned_bitPosCanWrapAroundWordSize)
{
	BitStream stream;

	for(uint i=0; i < 10; ++i)
		stream.writeUnsigned(0, 24);

	ASSERT_EQ(stream.tell(), 24*10);
}

TEST_F(BitStreamFixture, readAndWrite_dataIsCorrect)
{
	BitStream stream;

	u32 input[3] = { 0xFABB1, 0xBABE3, 0xD17 };
	u32 output[3] = {};
	stream.writeUnsigned(input[0], 20);
	stream.writeUnsigned(input[1], 20);
	stream.writeUnsigned(input[2], 20);
	const u32 writtenBits = stream.tell();

	stream.seek(0);

	output[0] = stream.readUnsigned(20);
	output[1] = stream.readUnsigned(20);
	output[2] = stream.readUnsigned(20);
	const u32 readBits = stream.tell();

	ASSERT_EQ(writtenBits, readBits);

	ASSERT_EQ(input[0], output[0]);
	ASSERT_EQ(input[1], output[1]);
	ASSERT_EQ(input[2], output[2]);
}

TEST_F(BitStreamFixture, writeUnsigned_handlesWordWrap)
{
	BitStream stream;
	const BitWordType Pattern = 0xC0C0A;

	for (uint i = 0; i < 10; ++i)
		stream.writeUnsigned(Pattern, 20);
	
	const u32 writtenBits = stream.tell();
	stream.seek(0);

	for (uint i = 0; i < 10; ++i)
		ASSERT_EQ(stream.readUnsigned(20), Pattern);
}

TEST_F(BitStreamFixture, writeUnsigned_overwritesAndClearsPreviousData)
{
	BitStream stream;
	const BitWordType Pattern = 0x74747;

	for (uint i = 0; i < 128; ++i)
	{
		stream.seek(i);
		stream.writeUnsigned(Pattern, 20);
		
		stream.seek(i);
		ASSERT_EQ(stream.readUnsigned(20), Pattern);

		stream.seek((i/32) * 32);
		const u32 bitOffset = i % 32;
		const u32 earlier = stream.readUnsigned(bitOffset);
		const u32 numOnes = bitword::countSetBits(earlier);
		ASSERT_EQ(numOnes, bitOffset);
	}
}


}