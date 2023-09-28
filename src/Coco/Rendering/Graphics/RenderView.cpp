#include "Renderpch.h"
#include "RenderView.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Material.h"
#include "Image.h"
#include "Buffer.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	RenderTarget::RenderTarget() : 
		RenderTarget(Ref<Image>(), Vector4::Zero)
	{}

	RenderTarget::RenderTarget(Ref<Image> mainImage, const Vector4& clearValue) :
		MainImage(mainImage),
		ResolveImage(),
		ClearValue(clearValue)
	{}

	void RenderTarget::SetColorClearValue(const Color& clearColor)
	{
		Color gammaColor = clearColor.AsLinear();
		ClearValue = Vector4(gammaColor.R, gammaColor.G, gammaColor.B, gammaColor.A);
	}

	void RenderTarget::SetDepthClearValue(double depthClearValue)
	{
		ClearValue.X = depthClearValue;
	}

	void RenderTarget::SetDepthStencilClearValue(double depthClearValue, uint8 stencilClearValue)
	{
		ClearValue.X = depthClearValue;
		ClearValue.Y = stencilClearValue;
	}

	MeshData::MeshData(
		uint64 id, 
		uint64 version, 
		const Ref<Buffer>& vertexBuffer, 
		uint64 vertexCount, 
		const Ref<Buffer>& indexBuffer,
		const BoundingBox& bounds) :
		ID(id),
		Version(version),
		VertexBuffer(vertexBuffer),
		VertexCount(vertexCount),
		IndexBuffer(indexBuffer),
		Bounds(bounds)
	{}

	RenderPassShaderData::RenderPassShaderData(uint64 id, uint64 version, const RenderPassShader& shaderData) :
		ID(id),
		Version(version),
		ShaderData(shaderData)
	{}

	ShaderData::ShaderData(uint64 id, uint64 version, const string& groupTag, const std::unordered_map<string, uint64>& passShaders) :
		ID(id),
		Version(version),
		GroupTag(groupTag),
		RenderPassShaders(passShaders)
	{}

	MaterialData::MaterialData(uint64 id, uint64 shaderID, const ShaderUniformData& uniformData) :
		ID(id),
		ShaderID(shaderID),
		UniformData(uniformData)
	{}

	ObjectData::ObjectData(
		uint64 id, 
		const Matrix4x4& modelMatrix, 
		uint64 meshID, 
		uint64 indexOffset, 
		uint64 indexCount,
		uint64 materialID, 
		const RectInt& scissorRect,
		const BoundingBox& bounds,
		SharedRef<ExtraObjectData> extraData) :
		ID(id),
		ModelMatrix(modelMatrix),
		MeshID(meshID),
		IndexOffset(indexOffset),
		IndexCount(indexCount),
		MaterialID(materialID),
		ScissorRect(scissorRect),
		Bounds(bounds),
		ExtraData(extraData)
	{}

	RenderView::RenderView() :
		_viewportRect(),
		_scissorRect(),
		_viewMat(),
		_projectionMat(),
		_frustum(),
		_samples(),
		_renderTargets(),
		_meshDatas(),
		_renderPassShaderDatas(),
		_shaderDatas(),
		_materialDatas(),
		_objectDatas()
	{}

	void RenderView::Setup(
		const RectInt& viewportRect, 
		const RectInt& scissorRect, 
		const Matrix4x4& viewMatrix, 
		const Matrix4x4& projectionMatrix,
		const ViewFrustum& frustum,
		MSAASamples samples, 
		const std::vector<RenderTarget>&renderTargets)
	{
		_viewportRect = viewportRect;
		_scissorRect = scissorRect;
		_viewMat = viewMatrix;
		_projectionMat = projectionMatrix;
		_frustum = frustum;
		_samples = samples;
		_renderTargets = renderTargets;
	}

	void RenderView::Reset()
	{
		_renderTargets.clear();
		_meshDatas.clear();
		_renderPassShaderDatas.clear();
		_shaderDatas.clear();
		_materialDatas.clear();
		_objectDatas.clear();
	}

	RenderTarget& RenderView::GetRenderTarget(size_t index)
	{
		Assert(index < _renderTargets.size())

		return _renderTargets.at(index);
	}

	uint64 RenderView::AddMesh(const Mesh& mesh)
	{
		uint64 meshID = mesh.GetID();

		if (!_meshDatas.contains(meshID))
		{
			_meshDatas.try_emplace(meshID, 
				meshID, 
				mesh.GetVersion(), 
				mesh.GetVertexBuffer(), 
				mesh.GetVertexCount(), 
				mesh.GetIndexBuffer(),
				mesh.GetBounds());
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
		uint64 shaderID = shader.GetID();

		if (!_shaderDatas.contains(shaderID))
		{
			std::unordered_map<string, uint64> passShaders;

			for (const RenderPassShader& passShader : shader.GetRenderPassShaders())
			{
				uint64 passShaderID = passShader.ID;
				_renderPassShaderDatas.try_emplace(passShaderID, passShaderID, passShader.Version, passShader);

				passShaders.try_emplace(passShader.PassName, passShaderID);
			}

			_shaderDatas.try_emplace(shaderID, shaderID, shader.GetVersion(), shader.GetGroupTag(), passShaders);
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

	uint64 RenderView::AddMaterial(const MaterialDataProvider& materialData)
	{
		uint64 id = materialData.GetMaterialID();

		if (!_materialDatas.contains(id))
		{
			Ref<Shader> shader = materialData.GetShader();
			uint64 shaderID = InvalidID;

			if (shader.IsValid())
			{
				shaderID = AddShader(*shader);
			}

			_materialDatas.try_emplace(id, id, shaderID, materialData.GetUniformData());
		}

		return id;
	}

	const MaterialData& RenderView::GetMaterialData(uint64 key) const
	{
		Assert(_materialDatas.contains(key))

		return _materialDatas.at(key);
	}

	uint64 RenderView::AddRenderObject(
		const Mesh& mesh, 
		uint32 submeshID, 
		const Matrix4x4& modelMatrix, 
		const MaterialDataProvider* material,
		const RectInt* scissorRect,
		SharedRef<ExtraObjectData> extraData)
	{
		SubMesh submesh;
		Assert(mesh.TryGetSubmesh(submeshID, submesh))
		return AddRenderObject(
			mesh,
			submesh.Offset, submesh.Count,
			modelMatrix,
			submesh.Bounds,
			material,
			scissorRect,
			extraData);
	}

	uint64 RenderView::AddRenderObject(
		const Mesh& mesh, 
		uint64 indexOffset, 
		uint64 indexCount, 
		const Matrix4x4& modelMatrix,
		const BoundingBox& bounds,
		const MaterialDataProvider* material, 
		const RectInt* scissorRect,
		SharedRef<ExtraObjectData> extraData)
	{
		uint64 meshID = AddMesh(mesh);
		uint64 materialID = material ? AddMaterial(*material) : InvalidID;

		uint64 objectID = _objectDatas.size();
		_objectDatas.emplace_back(
			objectID,
			modelMatrix,
			meshID,
			indexOffset,
			indexCount,
			materialID,
			scissorRect ? *scissorRect : _scissorRect,
			bounds.Transformed(modelMatrix),
			extraData);

		return objectID;
	}
}