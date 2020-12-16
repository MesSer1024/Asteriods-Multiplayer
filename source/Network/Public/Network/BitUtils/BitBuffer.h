// copyright Daniel Dahlkvist (c) 2020 [github.com/messer1024]
#pragma once

#include <Network/Core/Types.h>
#include <Network/BitUtils/BitWord.h>
#include <memory>

namespace dud
{

class BitBuffer final
{
public:
	enum NoInitType { NoInit };
	enum ZeroInitType { ZeroInit };
	enum OneInitType { OneInit };

	explicit BitBuffer(NoInitType t, u32 numBits)
		: _numBits(numBits)
		, _numWords(bitword::getNumWordsRequired(numBits))
		, _data(std::make_unique<BitWordType[]>(_numWords))
	{
	}

	explicit BitBuffer(ZeroInitType, u32 numBits) : BitBuffer(NoInitType{}, numBits) {
		memset(_data.get(), bitword::Zero, bitword::getNumBytesRequiredToRepresentWordBasedBitBuffer(numBits));
	}

	explicit BitBuffer(OneInitType, u32 numBits) : BitBuffer(NoInitType{}, numBits) {
		memset(_data.get(), ~0, bitword::getNumBytesRequiredToRepresentWordBasedBitBuffer(numBits));
	}

	inline u32 size() const { return _numWords * sizeof(BitWordType); }
	inline BitWordType* data() const { return _data.get(); }

	BitWordType* begin() const { return data(); }
	BitWordType* end() const { return data() + _numWords; }

private:
	u32 _numBits;
	u32 _numWords;
	std::unique_ptr<BitWordType[]> _data;
};

}
