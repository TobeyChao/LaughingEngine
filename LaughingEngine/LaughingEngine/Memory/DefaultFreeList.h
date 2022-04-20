#pragma once
#include "FreeList.h"
#include "PlacementNewEmbeddedLink.h"
#include "ConstantElementCountGrowth.h"

template <typename T>
struct DefaultFreeList
{
	using Type = FreeList<T, ConstantElementCountGrowth, PlacementNewEmbeddedLink<T>>;
};