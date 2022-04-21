#pragma once
#include "ECS/System.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "RenderTargetStorage.h"
#include "CommandContext.h"
#include "DataStruct.h"
#include "MainPassStorage.h"
#include "SkyPassStorage.h"
#include "PSOStorage.h"
#include "TextureStorage.h"

// ‰÷»æœµÕ≥
class RenderSystem : public System<MeshRenderer, Transform>
{
public:
	virtual void Update(float deltaTime) override
	{
		const D3D12_VIEWPORT& Viewport = RenderTargetStorage::GetInstance().MainViewport;
		const D3D12_RECT& Scissor = RenderTargetStorage::GetInstance().MainScissor;
		GraphicsContext& Context = GraphicsContext::Begin(L"Scene Render");

		Context.TransitionResource(Graphics::g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
		Context.SetRenderTarget(Graphics::g_SceneColorBuffer.GetRTV(), Graphics::g_SceneDepthBuffer.GetDSV());
		Context.ClearColor(Graphics::g_SceneColorBuffer, &Scissor);
		Context.ClearDepth(Graphics::g_SceneDepthBuffer);

		Context.SetRootSignature(PSOStorage::GetInstance().DefaultRS);
		Context.SetPipelineState(PSOStorage::GetInstance().DefaultPSO);
		Context.SetViewportAndScissorRect(Viewport, Scissor);
		Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			TextureStorage::GetInstance().TextureHeap.GetDescriptorHeapPointer());
		Context.SetDynamicConstantBufferView(1,
			sizeof(MainPassStorage::GetInstance().MainPassCB),
			&MainPassStorage::GetInstance().MainPassCB);

		for (auto& it : m_EntitiesCache)
		{
			MeshRenderer* renderer = GetComponent<MeshRenderer>(it);
			Transform* transform = GetComponent<Transform>(it);
			DrawRenderItems(*renderer, *transform, Context);
		}

		DrawSkybox(Context, Viewport, Scissor);

		Context.Finish();
	}

private:
	void DrawRenderItems(
		const MeshRenderer& ItemRenderer,
		const Transform& ItemTransform,
		GraphicsContext& Context)
	{
		Context.SetVertexBuffer(0, ItemRenderer.Mesh->VertexBufferGPU.VertexBufferView(0));
		Context.SetIndexBuffer(ItemRenderer.Mesh->IndexBufferGPU.IndexBufferView(0));
		Context.SetPrimitiveTopology(ItemRenderer.Mesh->PrimitiveType);

		ObjectConstants cb;
		cb.World = ItemTransform.World;
		Context.SetDynamicConstantBufferView(2, sizeof(cb), &cb);

		Context.SetDescriptorTable(0, TextureStorage::GetInstance().TextureHeap[ItemRenderer.TextureIndex]);

		Context.DrawIndexedInstanced(
			ItemRenderer.SubMesh->IndexCount,
			1, ItemRenderer.SubMesh->StartIndexLocation,
			ItemRenderer.SubMesh->BaseVertexLocation, 0);
	}

	void DrawSkybox(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor)
	{
		Context.SetRootSignature(PSOStorage::GetInstance().DefaultRS);
		Context.SetPipelineState(PSOStorage::GetInstance().SkyPSO);
		Context.SetViewportAndScissorRect(Viewport, Scissor);
		Context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			TextureStorage::GetInstance().TextureHeap.GetDescriptorHeapPointer());
		Context.SetDynamicConstantBufferView(1, sizeof(SkyPassStorage::GetInstance().SkyboxVSCB), &SkyPassStorage::GetInstance().SkyboxVSCB);
		Context.SetDynamicConstantBufferView(2, sizeof(SkyPassStorage::GetInstance().SkyboxPSCB), &SkyPassStorage::GetInstance().SkyboxPSCB);
		Context.SetDescriptorTable(3, TextureStorage::GetInstance().TextureHeap[5]);
		Context.DrawInstanced(3, 1, 0, 0);
	}
};