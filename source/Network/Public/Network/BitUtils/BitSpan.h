// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/CoreIncludes.h>

#include <Network/BitUtils/BitRangeZipper.h>
#include <Network/BitUtils/BitWord.h>

namespace dud
{

// BitSpan provide functionality to reason about a range of bits
// it does not own or manage any data/buffer [memory management is supposed to happen outside of this class]
// will attempt to "zero" any eventual dangling bits [seems like the best trade-off related to usability, performance and correctness]
class BitSpan final
{
public:
	BitSpan(const BitSpan&) = delete;
	void operator=(const BitSpan&) = delete;
	void operator=(BitSpan&&) = delete;
	BitSpan() = delete;

	inline BitSpan(BitWordType* data, u32 numBits)
		: _data(data)
		, _danglingMask(bitword::hasDanglingPart(numBits) ? bitword::getDanglingPart(numBits) : bitword::Ones)
		, _numWords(bitword::getNumWordsRequired(numBits))
		, _numBits(numBits)
	{
		DUD_ASSERT(numBits < 400000000); // sanity check against "-1 issues"

		clearDanglingBits();
	}

	inline void clearDanglingBits()
	{
		if (_numWords > 0 && _danglingMask != 0)
			_data[_numWords - 1] &= _danglingMask;
	}

	inline void clearAll() noexcept
	{
		foreachWord([](auto& a) { a = bitword::Zero; });
	}

	inline void setAll() noexcept
	{
		foreachWord([](auto& a) { a = bitword::Ones; });

		clearDanglingBits();
	}

	inline u32 countSetBits() {
		clearDanglingBits();

		u64 counter = 0;
		foreachWord([&counter](auto a) { counter += bitword::countSetBits(a); });
		return static_cast<u32>(counter);
	}

	inline void setBit(u32 bit)
	{
		DUD_ASSERT(bit < _numBits);

		auto& word = _data[bit / NumBitsInWord];
		bitword::setBit(word, bit % NumBitsInWord);
	}

	inline bool getBit(u32 bit) const
	{
		DUD_ASSERT(bit < _numBits);

		auto word = _data[bit / NumBitsInWord];
		return bitword::getBit(word, bit % NumBitsInWord);
	}

	template<typename WordAction>
	inline void foreachWord(WordAction&& action) noexcept {
		auto it = _data;
		auto end = it + _numWords;

		while (it != end)
		{
			action(*it);
			it++;
		}
	}

	template<typename BitAction>
	inline void foreachSetBit(BitAction&& action) noexcept {
		u32 it = 0;
		clearDanglingBits();

		foreachWord([&it, bitAction = std::forward<BitAction&&>(action)](auto word) {
			bitword::foreachOne(word, bitAction, it * NumBitsInWord);
			it++;
		});
	}

	inline bool operator==(const BitSpan& other)
	{
		DUD_ASSERT(_numBits == other._numBits);

		auto it = _data;
		auto otherIt = other._data;
		const auto end = _data + (_danglingMask ? _numWords - 1 : _numWords);

		while (it != end)
		{
			if (*it != *otherIt)
				return false;

			it++;
			otherIt++;
		}

		if (_danglingMask)
		{
			BitWordType value = *it ^ *otherIt;
			value &= _danglingMask;
			return value == 0;
		}

		return true;
	}

	inline void operator|=(const BitSpan& other)
	{
		DUD_ASSERT(_numBits == other._numBits);

		BitRangeZipper zipper(_data, other._data, _numBits);
		zipper.foreachWord([](auto& a, auto b) { a |= b; });

		clearDanglingBits();
	}

	inline void operator&=(const BitSpan& other)
	{
		DUD_ASSERT(_numBits == other._numBits);

		BitRangeZipper zipper(_data, other._data, _numBits);
		zipper.foreachWord([](auto& a, auto b) { a &= b; });

		clearDanglingBits();
	}

	inline void operator^=(const BitSpan& other)
	{
		DUD_ASSERT(_numBits == other._numBits);

		BitRangeZipper zipper(_data, other._data, _numBits);
		zipper.foreachWord([](auto& a, auto b) { a ^= b; });

		clearDanglingBits();
	}

private:
	BitWordType* _data;
	BitWordType _danglingMask;

	u32 _numWords;
	u32 _numBits;
};

}
