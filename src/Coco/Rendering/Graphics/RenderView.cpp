#include "Renderpch.h"
#include "RenderView.h"
#include "../Mesh.h"
#include "../Shader.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image) : 
		RenderTarget(image, Color::ClearBlack)
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Vector4 clearValue) :
		Image(image),
		ClearValue(clearValue)
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Color clearColor) :
		Image(image)
	{
		Color gammaColor = clearColor.AsGamma();
		ClearValue = Vector4(gammaColor.R, gammaColor.G, gammaColor.B, gammaColor.A);
	}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, double depthClearValue) :
		RenderTarget(image, Vector4(depthClearValue, 0, 0, 0))
	{}

	RenderTarget::RenderTarget(const Ref<Rendering::Image>& image, Vector2 depthStencilClearValue) :
		RenderTarget(image, Vector4(depthStencilClearValue.X, depthStencilClearValue.Y, 0, 0))
	{}

	MeshData::MeshData(
		uint64 id, 
		uint64 version, 
		const Ref<Buffer>& vertexBuffer, 
		uint64 vertexCount, 
		const Ref<Buffer>& indexBuffer, 
		uint32 firstIndexOffset, 
		uint32 indexCount) :
		ID(id),
		Version(version),
		VertexBuffer(vertexBuffer),
		VertexCount(vertexCount),
		IndexBuffer(indexBuffer),
		FirstIndexOffset(firstIndexOffset),
		IndexCount(indexCount)
	{}

	RenderPassShaderData::RenderPassShaderData(uint64 id, uint64 version, const RenderPassShader& shaderData) :
		ID(id),
		Version(version),
		ShaderData(shaderData)
	{}

	ShaderData::ShaderData(uint64 id, const string& groupTag, const std::unordered_map<string, uint64>& passShaders) :
		ID(id),
		GroupTag(groupTag),
		RenderPassShaders(passShaders)
	{}

	ObjectData::ObjectData(uint64 id, const Matrix4x4& modelMatrix, uint64 meshID, uint64 shaderID) :
		ID(id),
		ModelMatrix(modelMatrix),
		MeshID(meshID),
		ShaderID(shaderID)
	{}

	RenderView::RenderView(
		const RectInt& viewportRect, 
		const RectInt& scissorRect,
		const Matrix4x4& viewMatrix,
		const Matrix4x4& projectionMatrix,
		const std::vector<RenderTarget>& renderTargets) :
		_viewportRect(viewportRect),
		_scissorRect(scissorRect),
		_viewMat(viewMatrix),
		_projectionMat(projectionMatrix),
		_renderTargets(renderTargets)
	{}

	RenderTarget& RenderView::GetRenderTarget(size_t index)
	{
		Assert(index < _renderTargets.size())

		return _renderTargets.at(index);
	}

	uint64 RenderView::AddMesh(const Mesh& mesh, uint32 submeshID)
	{
		std::hash<const Mesh*> hasher;
		uint64 meshID = hasher(&mesh);

		if (!_meshDatas.contains(meshID))
		{
			SubMesh submesh{};
			Assert(mesh.TryGetSubmesh(submeshID, submesh))

			_meshDatas.try_emplace(meshID, meshID, 0, mesh.GetVertexBuffer(), mesh.GetVertexCount(), mesh.GetIndexBuffer(), submesh.Offset, submesh.Count);
		}

		return meshID;
	}

	const MeshData& RenderView::GetMeshData(uint64 key) const
	{
		Assert(_meshDatas.contains(key))

		return _meshDatas.at(key);
	}

	uint64 RenderView::AddShader(const Shader& shader)
	{
		std::hash<const Shader*> hasher;
		uint64 shaderID = hasher(&shader);

		if (!_shaderDatas.contains(shaderID))
		{
			std::unordered_map<string, uint64> passShaders;

			for (const RenderPassShader& passShader : shader.GetRenderPassShaders())
			{
				uint64 passShaderID = _renderPassShaderDatas.size();
				_renderPassShaderDatas.try_emplace(passShaderID, passShaderID, 0, passShader);

				passShaders.try_emplace(passShader.PassName, passShaderID);
			}

			_shaderDatas.try_emplace(shaderID, shaderID, shader.GetGroupTag(), passShaders);
		}

		return shaderID;
	}

	const ShaderData& RenderView::GetShaderData(uint64 key) const
	{
		Assert(_shaderDatas.contains(key))

		return _shaderDatas.at(key);
	}

	const RenderPassShaderData& RenderView::GetRenderPassShaderData(uint64 key) const
	{
		Assert(_renderPassShaderDatas.contains(key))

		return _renderPassShaderDatas.at(key);
	}

	void RenderView::AddRenderObject(const Mesh& mesh, uint32 submeshID, const Shader& shader, const Matrix4x4& modelMatrix)
	{
		uint64 meshID = AddMesh(mesh, submeshID);
		uint64 shaderID = AddShader(shader);

		uint64 objectID = _objectDatas.size();
		_objectDatas.emplace_back(objectID, modelMatrix, meshID, shaderID);
	}
}