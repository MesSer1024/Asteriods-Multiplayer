// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/BitUtils/BitWord.h>
#include <Network/CoreIncludes.h>

namespace dud
{
    // <Example> -- #TODO : add code generator that generates this enum
    enum class InputConcept
    {
        RotateLeft,
        RotateRight,
        Thrust,
        Action,
        LAST_ENTRY,
    };

    static_assert(static_cast<u32>(InputConcept::LAST_ENTRY) <= 32);

    struct InputState
    {
        void reset() { pressedConcepts = bitword::Zero; }

        void setPressed(InputConcept id) { bitword::setBit(pressedConcepts, static_cast<u32>(id)); }

        bool isPressed(InputConcept id) { return bitword::getBit(pressedConcepts, static_cast<u32>(id)); }

        bool isEmpty() { return pressedConcepts == bitword::Zero; }

        template<class Action>
        void foreachPressed(Action&& action)
        {
            bitword::foreachOne(pressedConcepts, [action = std::forward<decltype(action)>(action)](u32 bitIdx) {
                InputConcept inputConcept = static_cast<InputConcept>(bitIdx);
                action(inputConcept);
            });
        }

        BitWordType pressedConcepts = bitword::Zero;
    };

    // </Example>
}