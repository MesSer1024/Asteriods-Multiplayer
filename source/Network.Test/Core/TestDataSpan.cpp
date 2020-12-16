// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitStream.h>

#include <gtest/gtest.h>
#include <vector>

namespace dud
{

	class DataSpanFixture : public testing::Test
	{
	public:
	protected:
		void SetUp() override {
		}

		void TearDown() override {
		}
	};

	TEST_F(DataSpanFixture, NumWords_AffectedByCtorArgument)
	{
		u64 data[13];
		DataSpan<u64> span(&data[0], 7);

		ASSERT_EQ(span.numWords(), 7u);
	}

	TEST_F(DataSpanFixture, Size_AffectedByByteSizeOfWord)
	{
		const u32 WordCount = 13;

		{
			DataSpan<u64> span(nullptr, WordCount);
			ASSERT_EQ(span.size(), sizeof(u64)*WordCount);
		}
		{
			DataSpan<u32> span(nullptr, WordCount);
			ASSERT_EQ(span.size(), sizeof(u32) * WordCount);
		}
		{
			DataSpan<u16> span(nullptr, WordCount);
			ASSERT_EQ(span.size(), sizeof(u16) * WordCount);
		}
		{
			DataSpan<u8> span(nullptr, WordCount);
			ASSERT_EQ(span.size(), sizeof(u8) * WordCount);
		}
		{
			DataSpan<bool> span(nullptr, WordCount);
			ASSERT_EQ(span.size(), sizeof(bool) * WordCount);
		}
	}

	TEST_F(DataSpanFixture, ForLoop_CanAffectRelevantWords)
	{
		u64 data[13]{};
		DataSpan<u64> span(&data[0], 7);

		{
			uint i = 0;
			for (auto& word : span)
			{
				word = 11 + i;
				i++;
			}
		}

		for (uint i = 0; i < 13; ++i)
		{
			if (i < 7)
				ASSERT_EQ(data[i], 11 + i);
			else
				ASSERT_EQ(data[i], 0u);
		}
	}


}

