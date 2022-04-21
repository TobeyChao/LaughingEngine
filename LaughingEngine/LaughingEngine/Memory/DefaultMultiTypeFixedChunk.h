#ifndef DEFAULTMULTITYPEFIXEDCHUNK_
#define DEFAULTMULTITYPEFIXEDCHUNK_
#include "ConstantChunkSizeGrowth.h"
#include "FixedChunkCacheFriendlyLink.h"
#include "MultiTypeFixedChunk.h"

struct DefaultMultiTypeFixedChunk
{
	using Type = MultiTypeFixedChunk<ConstantChunkSizeGrowth, FixedChunkCacheFriendlyLink>;
};
#endif // DEFAULTMULTITYPEFIXEDCHUNK_
