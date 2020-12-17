// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#include <Network/Game/PlayerInput.h>

#include <gtest/gtest.h>

namespace dud
{
	class PlayerInputFixture : public testing::Test
	{
	public:
	protected:
		void SetUp() override {
		}

		void TearDown() override {
		}
	};

	using KeyboardStateFixture = PlayerInputFixture;

	TEST_F(KeyboardStateFixture, ctor_pressedKeysAreEmpty)
	{
		KeyboardState state;

		ASSERT_EQ(state.pressedConcepts, BitWordType{});
	}

	TEST_F(KeyboardStateFixture, reset_clearsState)
	{
		KeyboardState state;

		state.pressedConcepts = 0xfafafa;

		state.reset();
		ASSERT_EQ(state.pressedConcepts, BitWordType{});
	}

	TEST_F(KeyboardStateFixture, setAndGetPressedKeys)
	{
		KeyboardState state;
		InputConcept pressed[] = { InputConcept::RotateRight, InputConcept::RotateLeft, InputConcept::Action, InputConcept::Thrust };

		for (auto& c : pressed)
		{
			ASSERT_FALSE(state.isPressed(c));
			state.setPressed(c);
			ASSERT_TRUE(state.isPressed(c));
		}
	}

	TEST_F(KeyboardStateFixture, foreachPressedKey_returnsRightKey)
	{
		KeyboardState state;
		InputConcept pressed[] = { InputConcept::RotateRight, InputConcept::Action };

		for (auto& c : pressed)
			state.setPressed(c);

		state.foreachPressed([&pressed](InputConcept key) {
			ASSERT_TRUE(key == pressed[0] || key == pressed[1]);
			});
	}
}

