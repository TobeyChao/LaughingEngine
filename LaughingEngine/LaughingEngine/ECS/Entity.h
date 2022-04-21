#ifndef ENTITY_
#define ENTITY_
#include "TypeID.h"
#include "Memory/ChunkHandle.h"
#include <unordered_set>

class Entity
{
public:
	EntityID EntityID;
	ChunkHandle* MemHandle;
	size_t Hash;
	size_t PoolIndex;
	std::unordered_set<size_t> ComponentHash;

	bool IsValid;
};
#endif // ENTITY_
