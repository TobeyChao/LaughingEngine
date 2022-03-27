#include "Display.h"
#include "Utility.h"
#include "ColorBuffer.h"
#include "GameTimer.h"
#include "GraphicsCore.h"
#include "GraphicsCommon.h"
#include "GameCore.h"
#include "CommandContext.h"
#include "BufferManager.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "CommandListManager.h"

#include <cmath>

#pragma comment(lib, "dxgi.lib") 

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 600; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

// Shader
#include "CompiledShaders/ScreenQuadPresentVS.h"
#include "CompiledShaders/PresentSDRPS.h"

// Back Buffer的数量
#define SWAP_CHAIN_BUFFER_COUNT 3

// 交换链的格式
DXGI_FORMAT SwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;

namespace
{
	float s_FrameTime = 0.0f;
	uint64_t s_FrameIndex = 0;
	int64_t s_FrameStartTick = 0;

	bool s_EnableVSync = false;

	uint64_t s_CurFrameFence = 0;
	uint8_t s_NumFrameUploaded = 0;
	uint8_t s_FrameResourceNumLimit = 3;
	std::queue<uint64_t> s_FrameFenceQueue;
	bool s_EnableFrameResourceNumLimit = true;
}

namespace Graphics
{
	// 将SceneColorBuffer的内容应用颜色校正后绘制到交换链的缓冲区
	void PreparePresentSDR();
	void PreparePresentHDR();

	uint32_t g_NativeWidth = 0;
	uint32_t g_NativeHeight = 0;
	uint32_t g_DisplayWidth = 1280;
	uint32_t g_DisplayHeight = 720;

	enum class eResolution
	{
		k720p,
		k900p,
		k1080p,
		k1440p,
		k1800p,
		k2160p
	};

	// 原生分辨率就是colorbuffer的大小
	eResolution nativeResolution = eResolution::k720p;
	// 显示分辨率就是DisplayPlane的大小
	eResolution displayResolution = eResolution::k720p;

	ColorBuffer g_DisplayPlane[SWAP_CHAIN_BUFFER_COUNT];

	UINT g_CurrentBuffer = 0;

	IDXGISwapChain1* s_SwapChain1 = nullptr;

	RootSignature s_PresentRS;
	GraphicsPiplelineState PresentSDRPS(L"Core: PresentSDR");
	GraphicsPiplelineState PresentHDRPS(L"Core: PresentHDR");
	DescriptorHeap CommonHeap;

	void ResolutionToUINT(eResolution resolution, uint32_t& width, uint32_t& height)
	{
		switch (resolution)
		{
		case Graphics::eResolution::k720p:
			width = 1280;
			height = 720;
			break;
		case Graphics::eResolution::k900p:
			width = 1600;
			height = 900;
			break;
		case Graphics::eResolution::k1080p:
			width = 1920;
			height = 1080;
			break;
		case Graphics::eResolution::k1440p:
			width = 2560;
			height = 1440;
			break;
		case Graphics::eResolution::k1800p:
			width = 3200;
			height = 1800;
			break;
		case Graphics::eResolution::k2160p:
			width = 3840;
			height = 2160;
			break;
		default:
			width = 1280;
			height = 720;
			break;
		}
	}

	void SetNativeResolution()
	{
		uint32_t nativeWidth = 0, nativeHeight = 0;
		ResolutionToUINT(nativeResolution, nativeWidth, nativeHeight);
		if (g_NativeWidth == nativeWidth && g_NativeHeight == nativeHeight)
		{
			return;
		}
		g_NativeWidth = nativeWidth;
		g_NativeHeight = nativeHeight;
		g_CommandManager.IdleGPU();
		InitializeRenderingBuffers(g_NativeWidth, g_NativeHeight);
	}

	void PreparePresentSDR()
	{
		GraphicsContext& context = GraphicsContext::Begin(L"PreparePresentSDR");
		context.SetRootSignature(s_PresentRS);
		context.SetPipelineState(PresentSDRPS);
		context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, CommonHeap.GetDescriptorHeapPointer());
		context.SetDescriptorTable(0, CommonHeap[0]);
		context.TransitionResource(g_DisplayPlane[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET);
		context.SetRenderTarget(g_DisplayPlane[g_CurrentBuffer].GetRTV());
		context.SetViewportAndScissorRect(0, 0, g_DisplayWidth, g_DisplayHeight);
		context.DrawInstanced(3, 1);
		context.TransitionResource(g_DisplayPlane[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT);
		s_CurFrameFence = context.Finish();
		s_FrameFenceQueue.push(s_CurFrameFence);
	}

	void PreparePresentHDR()
	{

	}

	uint64_t GetFrameCount()
	{
		return s_FrameIndex;
	}

	float GetFrameTime()
	{
		return s_FrameTime;
	}

	float GetFrameRate()
	{
		return s_FrameTime == 0.0f ? 0.0f : 1.0f / s_FrameTime;
	}
}

namespace Display
{
	using namespace Graphics;

	void Initialize()
	{
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
		ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = g_DisplayWidth;
		swapChainDesc.Height = g_DisplayHeight;
		swapChainDesc.Format = SwapChainFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		dxgiFactory->CreateSwapChainForHwnd(g_CommandManager.GetCommandQueue(),
			Game::g_hWnd,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			&s_SwapChain1);

		for (uint32_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> dispalyPlane;
			ThrowIfFailed(s_SwapChain1->GetBuffer(i, IID_PPV_ARGS(dispalyPlane.GetAddressOf())));
			g_DisplayPlane[i].CreateFromSwapChain(L"Primary SwapChain Buffer", dispalyPlane.Detach());
		}

		s_PresentRS.Reset(4, 0);
		s_PresentRS[0].InitAsDescriptorTable(2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
		s_PresentRS[1].InitAsConstants(6, 0);
		s_PresentRS[2].InitAsShaderResourceView(2);
		s_PresentRS[3].InitAsDescriptorTable(2, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0);
		s_PresentRS.Finalize(L"PresentRS");

		PresentSDRPS.SetRootSignature(s_PresentRS);
		PresentSDRPS.SetRasterizerState(RasterizerTwoSided);
		PresentSDRPS.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		PresentSDRPS.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
		PresentSDRPS.SetSampleMask(UINT_MAX);
		PresentSDRPS.SetInputLayout(0, nullptr);
		PresentSDRPS.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		PresentSDRPS.SetVertexShader(g_pScreenQuadPresentVS, sizeof(g_pScreenQuadPresentVS));
		PresentSDRPS.SetPixelShader(g_pPresentSDRPS, sizeof(g_pPresentSDRPS));
		PresentSDRPS.SetRenderTargetFormat(SwapChainFormat, DXGI_FORMAT_UNKNOWN);
		PresentSDRPS.Finalize();

		SetNativeResolution();

		CommonHeap.Create(L"Common Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096);
		DescriptorHandle handle = CommonHeap.Alloc();
		D3D12_CPU_DESCRIPTOR_HANDLE SourceTextures[] =
		{
			g_SceneColorBuffer.GetSRV()
		};
		// The SrcDescriptorRangeStart parameter must be in a non shader-visible descriptor heap.
		// This is because shader-visible descriptor heaps may be created in WRITE_COMBINE memory or GPU local memory,
		// which is prohibitively slow to read from. If your application manages descriptor heaps
		// via copying the descriptors required for a given pass or frame from local "storage" descriptor heaps
		// to the GPU-bound descriptor heap, then use shader-opaque heaps for the storage heaps and copy into the GPU-visible heap as required.
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-copydescriptorssimple
		g_Device->CopyDescriptorsSimple(1, handle, SourceTextures[0], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void Shutdown()
	{
		CommonHeap.Destroy();

		s_SwapChain1->SetFullscreenState(FALSE, nullptr);
		s_SwapChain1->Release();
		for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
		{
			g_DisplayPlane[i].Destroy();
		}

		s_PresentRS.Destroy();
		PresentSDRPS.Destroy();
		PresentHDRPS.Destroy();
	}

	void Resize(uint32_t width, uint32_t height)
	{
		Utility::Printf(L"width:%d height:%d\n", width, height);
	}

	void Update()
	{
		// 添加FrameResource预提交数量限制
		if (s_EnableFrameResourceNumLimit)
		{
			if (s_FrameFenceQueue.size() >= s_FrameResourceNumLimit)
			{
				uint64_t fence = s_FrameFenceQueue.front();
				g_CommandManager.WaitForFence(fence);
				s_FrameFenceQueue.pop();
			}
		}
	}

	void Present()
	{
		PreparePresentSDR();

		UINT PresentInterval = s_EnableVSync ? 1u : 0u;

		s_SwapChain1->Present(PresentInterval, 0);

		g_CurrentBuffer = (g_CurrentBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

		int64_t CurrentTick = GameTimer::GetCurrentTick();

		s_FrameTime = (float)GameTimer::DeltaTimeSeconds(s_FrameStartTick, CurrentTick);

		s_FrameStartTick = CurrentTick;

		++s_FrameIndex;
	}
}