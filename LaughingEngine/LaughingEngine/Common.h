#pragma once
namespace Math
{
	auto AlignUpWithMask(auto value, size_t mask)
	{
		return ((size_t)value + mask) & ~mask;
	}

	auto AlignUp(auto value, size_t alignment)
	{
		return AlignUpWithMask(value, alignment - 1);
	}

	auto AlignDownWithMask(auto value, size_t mask)
	{
		return ((size_t)value) & ~mask;
	}

	auto AlignDown(auto value, size_t alignment)
	{
		return AlignDownWithMask(value, alignment - 1);
	}
}