// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/BitUtils/BitBuffer.h>
#include <Network/BitUtils/BitWord.h>
#include <algorithm>

namespace dud
{
	template<class DataWord>
	struct DataSpan
	{
		DataSpan(DataWord* begin, u32 size)
			: _begin(begin)
			, _end(begin + size)
		{
		}

		DataWord* begin() { return _begin; }
		DataWord* end() { return _end; }

		u32 numWords() {
			u32 numWords = static_cast<u32>(std::distance(_begin, _end));
			return numWords;
		}

		u32 size() {
			u32 wordCount = numWords();
			return wordCount * sizeof(DataWord);
		}

		DataWord* data() { return _begin; }

		DataWord* _begin;
		DataWord* _end;
	};

	class BitStream final
	{
		enum { NumMaxBitsBuffer = 2000 * 8 }; // Enough to handle MTU

	public:
		BitStream()
			: _buffer{}
			, _bitPos{}
			, _endOfStream{}
		{
		}

		u32 tell() { return _bitPos; }
		void seek(u32 pos) { _bitPos = pos; }

		float32 readFloat(u32 numBits)
		{
#ifdef DEBUG
			DUD_NOT_DEFINED;
#else
			DUD_NOT_DEFINED();
#endif
			return {}; // #todo
		}

		void writeUnsigned(u32 data, u32 numBits)
		{
			if (numBits == 0)
				return;

			DUD_ASSERT(numBits <= 32);
			DUD_ASSERT(_bitPos + numBits < NumMaxBitsBuffer)

			const u32 startWordIdx = bitword::getWordIdxForBit(_bitPos);
			const u32 endWordIdx = bitword::getWordIdxForBit(_bitPos + numBits - 1);
			const u32 bitOffset = _bitPos % NumBitsInWord;

			// populate output word
			if (endWordIdx != startWordIdx)
			{
				const u32 bitsInFirstChunk = NumBitsInWord - bitOffset;

				BitWordType& word1 = _buffer[startWordIdx];
				BitWordType& word2 = _buffer[startWordIdx + 1];

				bitword::writeBits(word1, data, bitOffset, NumBitsInWord);
				bitword::writeBits(word2, data >> bitsInFirstChunk, 0, numBits - bitsInFirstChunk);
			}
			else
			{
				bitword::writeBits(_buffer[startWordIdx], data, bitOffset, bitOffset + numBits);
			}

			// advance bit-pointer
			_bitPos += numBits;
			_endOfStream = _bitPos;
		}

		u32 readUnsigned(u32 numBits)
		{
			if (numBits == 0)
				return 0;

			DUD_ASSERT(numBits <= 32);
			DUD_ASSERT(_bitPos + numBits <= _endOfStream);

			const u32 startWordIdx = bitword::getWordIdxForBit(_bitPos);
			const u32 endWordIdx = bitword::getWordIdxForBit(_bitPos + numBits - 1);
			const u32 bitOffset = _bitPos % NumBitsInWord;

			BitWordType output;

			// populate output word
			if (endWordIdx != startWordIdx)
			{
				const u32 bitsInFirstChunk = NumBitsInWord - bitOffset;
				output = bitword::readBits(_buffer[startWordIdx], bitOffset, NumBitsInWord);
				output |= (bitword::readBits(_buffer[startWordIdx + 1], numBits - bitsInFirstChunk) << bitsInFirstChunk);
			}
			else
			{
				output = bitword::readBits(_buffer[startWordIdx], bitOffset, bitOffset + numBits);
			}

			// advance bit-pointer
			_bitPos += numBits;

			return static_cast<u32>(output);
		}

		void populateFrom(DataSpan<u8> input)
		{
			constexpr u32 BufferSize = sizeof(_buffer);
			const u32 inputSize = input.size();
			const u32 byteCount = std::min(inputSize, BufferSize);

			memcpy(_buffer, input.data(), byteCount);

			_bitPos = byteCount * 8;
			_endOfStream = _bitPos;
		}

		u32 transferTo(DataSpan<u8> output)
		{
			constexpr u32 BufferSize = sizeof(_buffer);
			const u32 outputSize = output.size();
			const u32 writtenBytes = std::min(outputSize, BufferSize);

			memcpy(output.data(), _buffer, writtenBytes);

			return writtenBytes;
		}

	private:

		u32 _bitPos;
		u32 _endOfStream;
		BitWordType _buffer[bitword::getNumWordsRequired(NumMaxBitsBuffer)];
	};

}
