#pragma once
#include "ECS/System.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "RenderTargetStorage.h"
#include "CommandContext.h"
#include "DataStruct.h"
#include "MainPassStorage.h"
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

		Context.SetRootSignature(PSOStorage::GetInstance().m_DefaultRS);
		Context.SetPipelineState(PSOStorage::GetInstance().DefaultPSO);
		Context.SetViewportAndScissorRect(Viewport, Scissor);
		Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			TextureStorage::GetInstance().TextureHeap.GetDescriptorHeapPointer());
		Context.SetDescriptorTable(0, TextureStorage::GetInstance().TextureHeap[0]);
		Context.SetDynamicConstantBufferView(1,
			sizeof(MainPassStorage::GetInstance().MainPassCB),
			&MainPassStorage::GetInstance().MainPassCB);

		for (auto& it : m_EntitiesCache)
		{
			MeshRenderer* renderer = GetComponent<MeshRenderer>(it);
			Transform* transform = GetComponent<Transform>(it);
			DrawRenderItems(*renderer, *transform, Context);
		}

		Context.Finish();
	}

private:
	void DrawRenderItems(
		MeshRenderer& ItemRenderer,
		Transform& ItemTransform,
		GraphicsContext& Context)
	{
		Context.SetVertexBuffer(0, ItemRenderer.Mesh->VertexBufferGPU.VertexBufferView(0));
		Context.SetIndexBuffer(ItemRenderer.Mesh->IndexBufferGPU.IndexBufferView(0));
		Context.SetPrimitiveTopology(ItemRenderer.Mesh->PrimitiveType);

		ObjectConstants cb;
		cb.World = ItemTransform.World;
		Context.SetDynamicConstantBufferView(2, sizeof(cb), &cb);

		Context.DrawIndexedInstanced(
			ItemRenderer.SubMesh->IndexCount,
			1, ItemRenderer.SubMesh->StartIndexLocation,
			ItemRenderer.SubMesh->BaseVertexLocation, 0);
	}
};