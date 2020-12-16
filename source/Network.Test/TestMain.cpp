// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <gtest/gtest.h>

namespace dud
{

class TestRunner
{
public:
	int run(int argc, char** argv)
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
};

}

int main(int argc, char** argv)
{
	dud::TestRunner testRunner;
	return testRunner.run(argc, argv);
}