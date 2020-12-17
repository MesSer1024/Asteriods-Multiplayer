// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/CoreIncludes.h>
#include <intrin.h>
#include <functional>

namespace dud
{

using BitWordType = u64;

constexpr u32 NumBitsInWord = sizeof(BitWordType) * 8;

namespace bitword
{

constexpr BitWordType Zero = BitWordType{ 0 };
constexpr BitWordType Ones = BitWordType{ ~0ull };

constexpr BitWordType createMask(u32 firstBit, u32 lastBit)
{
	DUD_ASSERT(firstBit <= lastBit && firstBit < NumBitsInWord && lastBit <= NumBitsInWord);

	const BitWordType upper = lastBit == NumBitsInWord ? ~0ull : (1ull << lastBit) - 1;
	const BitWordType lower = (1ull << firstBit) - 1;

	const BitWordType mask = upper & ~lower;

	return mask;
}

constexpr BitWordType readBits(const BitWordType word, u32 lastBit)
{
	const BitWordType mask = createMask(0, lastBit);
	return word & mask;
}

constexpr BitWordType readBits(const BitWordType word, u32 firstBit, u32 lastBit)
{
	const BitWordType mask = createMask(firstBit, lastBit);
	return (word & mask) >> firstBit;
}

constexpr void writeBits(BitWordType& destination, const BitWordType source, u32 firstBit, u32 lastBit)
{
	const BitWordType mask = createMask(firstBit, lastBit);
	destination = destination & ~mask; // clear bits
	destination |= ((source << firstBit) & mask);
}

constexpr bool hasDanglingPart(u32 numBits)
{
	return (numBits % NumBitsInWord != 0);
}

constexpr u32 getWordIdxForBit(u32 bit) {
	return bit / NumBitsInWord;
}

constexpr u32 getNumWordsRequired(u32 numBits) {
	const u32 numWords = (numBits / NumBitsInWord) + hasDanglingPart(numBits);
	return numWords;
}

constexpr u32 getNumBytesRequiredToRepresentWordBasedBitBuffer(u32 numBits) {
	const u32 numWords = getNumWordsRequired(numBits);
	const u32 numBytes = numWords * sizeof(BitWordType);
	return numBytes;
}

constexpr BitWordType getDanglingPartMask(u32 numBits)
{
	numBits = numBits % NumBitsInWord;

	BitWordType value = (1ull << numBits) - 1;
	return value;
}

inline void clearBit(BitWordType& word, u32 bit)
{
	BitWordType mask = (1ull << bit);
	word &= ~mask;
}

inline void setBit(BitWordType& word, u32 bit)
{
	BitWordType mask = (1ull << bit);
	word = (word & ~mask) | mask;
}

inline void modifyBit(BitWordType& word, u32 bit, bool enabledFlag)
{
	if (enabledFlag)
		setBit(word, bit);
	else
		clearBit(word, bit);
}

inline bool getBit(BitWordType word, u32 bit)
{
	BitWordType mask = (1ull << bit);
	return word & mask;
}

inline u32 countSetBits(BitWordType word)
{
	return static_cast<u32>(__popcnt64(word));
}

template<class BitAction>
void foreachOne(BitWordType word, BitAction&& action, uint functorReportedBitOffset = 0)
{
	uint i = functorReportedBitOffset;

	while (word != 0ull)
	{
		if (word & 1ull)
			action(i);

		i++;
		word >>= 1;
	}
}

}
}