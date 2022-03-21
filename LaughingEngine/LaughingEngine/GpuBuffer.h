#pragma once

#include "GpuResource.h"

class GpuBuffer : public GpuResource
{
};

class ByteAddressBuffer : public GpuBuffer
{
};

class IndirectArgsBuffer : public ByteAddressBuffer
{
};

class StructuredBuffer : public GpuBuffer
{
};

class TypedBuffer : public GpuBuffer
{
};
