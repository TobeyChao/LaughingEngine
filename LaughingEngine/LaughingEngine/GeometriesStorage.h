#pragma once
#include "PCH.h"
#include "DataStruct.h"
#include "MeshGeometry.h"
#include "GeometryGenerator.h"
#include <DirectXColors.h>
#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>

class GeometriesStorage : public TSingleton<GeometriesStorage>
{
public:
	void Load()
	{
		{
			struct Vertex
			{
				XMFLOAT3 Position;
				XMFLOAT3 Normal;
				XMFLOAT3 Tangent;
				XMFLOAT2 TexCoord;
			};

			Assimp::Importer loader;
			aiMaterial* material = nullptr;
			aiString path;

			const aiScene* scene = loader.ReadFile("Assets/Meshes/cerberus.fbx", aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

			auto geo = std::make_unique<MeshGeometry>();
			geo->Name = L"Cerberus";

			std::vector<Vertex> vertices;
			std::vector<std::uint16_t> indices;

			size_t startIndexLocation = 0;
			size_t baseVertexLocation = 0;
			for (unsigned i = 0; i < scene->mNumMeshes; i++)
			{
				aiMesh* aimesh = scene->mMeshes[i];

				material = scene->mMaterials[aimesh->mMaterialIndex];

				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);

				std::vector<Vertex> subVertices(aimesh->mNumVertices);
				for (size_t i = 0; i < aimesh->mNumVertices; ++i)
				{
					auto& p = aimesh->mVertices[i];
					int uvChannelNum = aimesh->GetNumUVChannels();
					if (uvChannelNum >= 1)
					{
						auto& texC = aimesh->mTextureCoords[0][i];
						subVertices[i].TexCoord = XMFLOAT2{ texC.x, texC.y };
					}

					auto& normal = aimesh->mNormals[i];
					auto& tangent = aimesh->mTangents[i];
					subVertices[i].Position = { p.x, p.y, p.z };
					subVertices[i].Normal = { normal.x, normal.y, normal.z };
					subVertices[i].Tangent = { tangent.x, tangent.y, tangent.z };
				}
				std::vector<std::uint16_t> subIndices;
				for (unsigned k = 0; k < aimesh->mNumFaces; k++)
				{
					const struct aiFace* face = &aimesh->mFaces[k];
					for (unsigned m = 0; m < face->mNumIndices; m++)
					{
						int index = face->mIndices[m];
						subIndices.push_back(index);
					}
				}

				SubmeshGeometry submesh;
				submesh.IndexCount = (UINT)subIndices.size();
				submesh.StartIndexLocation = (UINT)startIndexLocation;
				submesh.BaseVertexLocation = (INT)baseVertexLocation;

				geo->DrawArgs[Utility::UTF8ToWideString(aimesh->mName.C_Str())] = submesh;

				baseVertexLocation += subVertices.size();
				startIndexLocation += subIndices.size();

				vertices.insert(vertices.end(), subVertices.begin(), subVertices.end());
				indices.insert(indices.end(), subIndices.begin(), subIndices.end());
			}

			const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
			const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

			ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
			CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

			ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
			CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

			geo->VertexBufferGPU.Create(L"fbx-VertexBufferGPU", sizeof(Vertex), (UINT)vertices.size(), geo->VertexBufferCPU->GetBufferPointer());
			geo->IndexBufferGPU.Create(L"fbx-IndexBufferGPU", sizeof(std::uint16_t), (UINT)indices.size(), geo->IndexBufferCPU->GetBufferPointer());

			Geometries[geo->Name] = std::move(geo);

			loader.FreeScene();
		}

		{
			struct Vertex
			{
				XMFLOAT3 Position;
				XMFLOAT2 TexCoord;
				XMFLOAT3 Normal;
				XMFLOAT4 Color;
			};

			auto geo = std::make_unique<MeshGeometry>();
			geo->Name = L"Box";

			GeometryGenerator geoGen;
			GeometryGenerator::MeshData model = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0);

			std::vector<Vertex> vertices(model.Vertices.size());

			for (size_t i = 0; i < model.Vertices.size(); ++i)
			{
				auto& p = model.Vertices[i];
				vertices[i].Position = { p.Position.x, p.Position.y, p.Position.z };
				vertices[i].TexCoord = p.TexC;
				vertices[i].Normal = p.Normal;
				vertices[i].Color = XMFLOAT4(DirectX::Colors::White);
			}

			std::vector<std::uint16_t> indices;
			indices.insert(indices.end(), std::begin(model.GetIndices16()), std::end(model.GetIndices16()));

			const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
			const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

			//{
			//	UploadBuffer uploadBufferV;
			//	uploadBufferV.Create(L"Temp uploadBufferV", vbByteSize);
			//	memcpy(uploadBufferV.Map(), vertices.data(), vbByteSize);
			//	uploadBufferV.Unmap();

			//	UploadBuffer uploadBufferI;
			//	uploadBufferI.Create(L"Temp uploadBufferI", ibByteSize);
			//	memcpy(uploadBufferI.Map(), indices.data(), ibByteSize);
			//	uploadBufferI.Unmap();

			//	geo->VertexBufferGPU.Create(L"MyGameApp::m_BoxVertexBuffer", sizeof(Vertex), (UINT)vertices.size(), uploadBufferV, 0);
			//	geo->IndexBufferGPU.Create(L"MyGameApp::m_BoxIndexBuffer", sizeof(sizeof(std::uint16_t)), (UINT)indices.size(), uploadBufferI, 0);
			//}

			ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
			CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

			ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
			CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

			geo->VertexBufferGPU.Create(L"MyGameApp::m_BoxVertexBuffer", sizeof(Vertex), (UINT)vertices.size(), geo->VertexBufferCPU->GetBufferPointer());
			geo->IndexBufferGPU.Create(L"MyGameApp::m_BoxIndexBuffer", sizeof(sizeof(std::uint16_t)), (UINT)indices.size(), geo->IndexBufferCPU->GetBufferPointer());

			SubmeshGeometry submesh;
			submesh.IndexCount = (UINT)indices.size();
			submesh.StartIndexLocation = 0;
			submesh.BaseVertexLocation = 0;

			geo->DrawArgs[L"default"] = submesh;
			Geometries[geo->Name] = std::move(geo);
		}
	}

	void Shutdown()
	{
		Geometries.clear();
	}

	// 所有加载的Mesh
	std::unordered_map<std::wstring, std::unique_ptr<MeshGeometry>> Geometries;
};
