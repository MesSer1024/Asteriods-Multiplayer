// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/CoreIncludes.h>
#include <Network/BitUtils/BitWord.h>

namespace dud
{

// utility class for being able to perform actions on two different "range of bits"
class BitRangeZipper final
{
public:
	BitRangeZipper(BitWordType* __restrict lhs, BitWordType* __restrict rhs, u32 numBits)
		: _lhs(lhs)
		, _rhs(rhs)
		, _danglingMask(bitword::hasDanglingPart(numBits) ? bitword::getDanglingPart(numBits) : bitword::Ones)
		, _numWords(bitword::getNumWordsRequired(numBits))
		, _numBits(numBits)
	{
		DUD_ASSERT(numBits < 400000000); // sanity check against "-1 issues"

		clearDanglingBits();
	}

	inline void clearDanglingBits()
	{
		if (_numWords > 0 && _danglingMask != 0) {
			_lhs[_numWords - 1] &= _danglingMask;
			_rhs[_numWords - 1] &= _danglingMask;
		}
	}

	// examples:
	// auto operatorOREq = [](auto& a, auto b) -> BitWordType { a |= b; }
	// zipper.foreachWord(operatorOrEq, danglingOrEq);
	template<class BitAction>
	inline void foreachWord(BitAction&& action) noexcept {
		auto lhs = _lhs;
		auto rhs = _rhs;
		auto end = lhs + _numWords;

		while (lhs != end)
		{
			action(*lhs, *rhs);
			lhs++;
			rhs++;
		}
	}

private:
	BitWordType* __restrict _lhs;
	BitWordType* __restrict _rhs;
	BitWordType _danglingMask;

	u32 _numWords;
	u32 _numBits;
};

}
