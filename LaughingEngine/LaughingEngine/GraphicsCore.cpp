#include "PCH.h"
#include "GraphicsCore.h"
#include "GraphicsCommon.h"
#include "CommandListManager.h"
#include "CommandContext.h"
#include "CommandContextManager.h"
#include "DescriptorAllocator.h"
#include "Display.h"
#include "BufferManager.h"

namespace Graphics
{
	using namespace Microsoft::WRL;
	using namespace Utility;

	ID3D12Device* g_Device = nullptr;
	CommandListManager g_CommandManager;
	ContextManager g_ContextManager;

	void EnableShaderBasedValidation()
	{
		ComPtr<ID3D12Debug> spDebugController0;
		ComPtr<ID3D12Debug1> spDebugController1;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0)));
		spDebugController0->EnableDebugLayer();
		//ThrowIfFailed(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1)));
		//spDebugController1->SetEnableGPUBasedValidation(true);
	}

	void Initialize()
	{
		ComPtr<ID3D12Device> pDevice;
		DWORD dxgiFactoryFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
		EnableShaderBasedValidation();
#endif // defined(DEBUG) || defined(_DEBUG)

		ComPtr<IDXGIFactory6> dxgiFactory;
		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

		ComPtr<IDXGIAdapter1> pAdapter;

		D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

		SIZE_T MaxSize = 0;
		for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx)
		{
			DXGI_ADAPTER_DESC1 desc;
			pAdapter->GetDesc1(&desc);

			// Is a software adapter?
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// Can create a D3D12 device?
			if (FAILED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pDevice.GetAddressOf()))))
				continue;

			// By default, search for the adapter with the most memory because that's usually the dGPU.
			if (desc.DedicatedVideoMemory < MaxSize)
				continue;

			MaxSize = desc.DedicatedVideoMemory;

			if (g_Device != nullptr)
				g_Device->Release();

			g_Device = pDevice.Detach();

			Utility::Printf(L"Selected GPU:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
		}

		g_CommandManager.Initialize(g_Device);

		InitializeCommonState();

		Display::Initialize();
	}

	void Shutdown()
	{
		g_CommandManager.IdleGPU();
		g_ContextManager.Shutdown();
		g_CommandManager.Shutdown();
		DescriptorAllocator::DestroyAll();
		DestroyCommonState();
		DestroyRenderingBuffers();
		Display::Shutdown();

#if defined(DEBUG) || defined(_DEBUG)
		ID3D12DebugDevice* debugInterface;
		if (SUCCEEDED(g_Device->QueryInterface(&debugInterface)))
		{
			debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
			debugInterface->Release();
		}

#endif // defined(DEBUG) || defined(_DEBUG)

		if (g_Device != nullptr)
		{
			g_Device->Release();
			g_Device = nullptr;
		}

#if defined(DEBUG) || defined(_DEBUG)
		IDXGIDebug1* dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
			dxgiDebug->Release();
		}
#endif // defined(DEBUG) || defined(_DEBUG)
	}
}