// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/BitUtils/BitStream.h>

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

}