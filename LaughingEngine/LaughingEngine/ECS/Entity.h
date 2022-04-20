#pragma once
#include "TypeID.h"
#include "Memory/ChunkHandle.h"
#include <unordered_set>

class Entity
{
public:
	EntityID EntityID;
	ChunkHandle* MemHandle;
	size_t Hash;
	std::unordered_set<size_t> ComponentHash;
};