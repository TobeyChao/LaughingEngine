#pragma once

#include "PCH.h"

class GpuResource
{
public:
	GpuResource()
		:
		m_GpuVirtualAddress(0),
		m_UsageState(D3D12_RESOURCE_STATE_COMMON),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{
	}

	GpuResource(ID3D12Resource* res,
		D3D12_RESOURCE_STATES currentState)
		:
		m_GpuVirtualAddress(0),
		m_pResource(res),
		m_UsageState(currentState),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{
	}

	~GpuResource()
	{
		Destroy();
	}

	virtual void Destroy()
	{
		m_pResource.Reset();
		m_GpuVirtualAddress = 0;
		++m_VersionID;
	}

	ID3D12Resource* GetRes() const
	{
		return m_pResource.Get();
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