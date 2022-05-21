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
class RenderSystem final : public System<MeshRenderer, Transform>
{
public:
	void Update(float deltaTime) override
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
		Context.SetDynamicConstantBufferView(1, sizeof(PassConstants), MainPassStorage::GetInstance().MainPassCB);

		for (const auto& it : m_EntitiesCache)
		{
			MeshRenderer* renderer = GetComponent<MeshRenderer>(it);
			Transform* transform = GetComponent<Transform>(it);
			DrawRenderItems(*renderer, *transform, Context);
		}

		Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, TextureStorage::GetInstance().TextureHeap.GetDescriptorHeapPointer());
		DrawSkybox(Context, Viewport, Scissor);
		Context.Finish();
	}

private:
	static void DrawRenderItems(const MeshRenderer& ItemRenderer, const Transform& ItemTransform, GraphicsContext& Context)
	{
		Context.SetVertexBuffer(0, ItemRenderer.Mesh->VertexBufferGPU.VertexBufferView(0));
		Context.SetIndexBuffer(ItemRenderer.Mesh->IndexBufferGPU.IndexBufferView(0));
		Context.SetPrimitiveTopology(ItemRenderer.Mesh->PrimitiveType);

		ObjectConstants cb;
		cb.World = ItemTransform.World;

		for (auto& [id, ref] : ItemRenderer.Material->Textures)
		{
			auto* program = ItemRenderer.Material->ShaderProgram->GetParam(id);
			assert(program != nullptr);
			Context.SetDynamicDescriptor(3, program->Index - 10, ref.GetSRV());
		}

		Context.SetDynamicConstantBufferView(2, sizeof(cb), &cb);
		const auto* subMesh = ItemRenderer.SubMesh;
		Context.DrawIndexedInstanced(subMesh->IndexCount, 1, subMesh->StartIndexLocation, subMesh->BaseVertexLocation, 0);
	}

	static void DrawSkybox(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor)
	{
		Context.SetRootSignature(PSOStorage::GetInstance().DefaultRS);
		Context.SetPipelineState(PSOStorage::GetInstance().SkyPSO);
		Context.SetViewportAndScissorRect(Viewport, Scissor);
		Context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, TextureStorage::GetInstance().TextureHeap.GetDescriptorHeapPointer());
		Context.SetDynamicConstantBufferView(1, sizeof(SkyPassStorage::GetInstance().SkyboxVSCB), &SkyPassStorage::GetInstance().SkyboxVSCB);
		Context.SetDynamicConstantBufferView(2, sizeof(SkyPassStorage::GetInstance().SkyboxPSCB), &SkyPassStorage::GetInstance().SkyboxPSCB);
		Context.SetDescriptorTable(3, TextureStorage::GetInstance().TextureHeap[5]);
		Context.DrawInstanced(3, 1, 0, 0);
	}
};