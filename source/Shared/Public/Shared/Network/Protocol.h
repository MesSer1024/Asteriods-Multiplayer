#pragma once

#include <Shared/Types.h>
#include <memory>

namespace dud
{
	constexpr u32 NumBitsInWord = 64;
	constexpr u32 NumMaxBytesInPacket = 2048;
	constexpr u32 NumMaxWordsInPacket = (NumMaxBytesInPacket * 8) / NumBitsInWord;

	constexpr u32 bitsToBytes(u32 bits)
	{
		constexpr u32 pow2 = 8;
		//DUD_ASSERT(pow2 && ((pow2 & (pow2 - 1)) == 0));
		u32 numBitsNormalized = (bits + pow2 - 1) & -pow2;
		return numBitsNormalized / pow2;
	}

	constexpr u32 bitsToWords(u32 bits)
	{
		constexpr u32 pow2 = 64;
		//DUD_ASSERT(pow2 && ((pow2 & (pow2 - 1)) == 0));
		u32 numBitsNormalized = (bits + pow2 - 1) & -pow2;
		return numBitsNormalized / pow2;
	}

	struct DudPacket
	{
		DudPacket()
			: buffer(new u8[NumMaxBytesInPacket])
			, bitsUsed(NumMaxBytesInPacket)
		{}

		std::unique_ptr<u8> buffer;
		u32 bitsUsed;
	};

	class BitStream
	{
	public:
		BitStream()
			: _internalBuffer{}
			, _bitPos(0)
		{}

		u32 tell() { return _bitPos; }

	protected:
		u64 _internalBuffer[NumMaxWordsInPacket];
		u32 _bitPos;
	};

	class BitWriter : BitStream
	{
	public:
		void transfer(void* destination)
		{
			const u32 numBytes = bitsToBytes(_bitPos);
			memcpy(destination, _internalBuffer, numBytes);
		}

		bool writeBool(bool flag)
		{
			u64 bitInWord = static_cast<u64>(_bitPos % NumBitsInWord);
			u64 otherBits = (~0ull) & ~(1ull << bitInWord);
			u64 flagAsWord = static_cast<u64>(flag);
		}

		u32 writeUnsigned(u32 data, u32 bits)
		{

		}

		u64 writeUnsigned64(u64 data, u32 bits)
		{

		}

		void flush()
		{
			u64& word = _internalBuffer[bitsToWords(_bitPos)];
			word = _cache;
		}

		inline void seek(u32 bitPos) 
		{ 
			_bitPos = bitPos; 
		}

	private:
		u64 _cache;
	};

	class BitReader : BitStream
	{
		void populate(void* source, u32 bitCount)
		{
			const u32 numBytes = bitsToBytes(bitCount);
			memcpy(_internalBuffer, source, numBytes);
			_bitPos = bitCount;
		}

		bool readBool()
		{

		}

		u32 readUnsigned(u32 bits)
		{

		}

		u64 readUnsigned64(u32 bits)
		{

		}

		inline void seek(u32 bitPos) 
		{ 
			_bitPos = bitPos; 
		}
	};
}
