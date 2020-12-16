// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/BitUtils/BitBuffer.h>

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
	public:
		BitStream()
			: _buffer{}
			, _bitPos{}
			, _unflushed{}
			, _numMaxBits(2048 * 8)
		{
		}

		u32 tell() { return _bitPos; }

	private:
		u32 _bitPos;
		u32 _numMaxBits;
		BitWordType _unflushed;
		BitWordType _buffer[bitword::getNumWordsRequired(2048 * 8)];
	};

}
