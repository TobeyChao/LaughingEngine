#pragma once
#include "PCH.h"

class GpuResource
{
	friend class CommandContext;
	friend class GraphicsContext;
	friend class ComputeContext;

public:
	GpuResource()
		:
		m_UsageState(D3D12_RESOURCE_STATE_COMMON),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1),
		m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
	{}

	GpuResource(ID3D12Resource* res, D3D12_RESOURCE_STATES currentState)
		:
		m_pResource(res),
		m_UsageState(currentState),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1),
		m_GpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
	{}

	virtual ~GpuResource()
	{
		Destroy();
	}

	virtual void Destroy()
	{
		m_pResource.Reset();
		m_GpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
		++m_VersionID;
	}

	inline ID3D12Resource* GetRes() const
	{
		return m_pResource.Get();
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const
	{
		return m_GpuVirtualAddress;
	}

	inline void SetState(D3D12_RESOURCE_STATES state)
	{
		m_UsageState = state;
	}

	inline void SetStateTarget(D3D12_RESOURCE_STATES state)
	{
		m_TransitioningState = state;
	}

	inline D3D12_RESOURCE_STATES CurState() const
	{
		return m_UsageState;
	}

	inline D3D12_RESOURCE_STATES TargetState() const
	{
		return m_TransitioningState;
	}


protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
	D3D12_RESOURCE_STATES m_UsageState;
	D3D12_RESOURCE_STATES m_TransitioningState;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
	uint32_t m_VersionID = 0;
};