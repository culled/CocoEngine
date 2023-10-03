#include "Renderpch.h"
#include "RenderView.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Material.h"
#include "ShaderUniformLayout.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const GlobalShaderUniformLayout RenderView::DefaultGlobalUniformLayout = GlobalShaderUniformLayout(
		{
			ShaderDataUniform("ProjectionMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4),
			ShaderDataUniform("ViewMatrix", ShaderStageFlags::Vertex, BufferDataType::Mat4x4)
		},
		{},
		{}
	);

	RenderView::RenderView() :
		_viewportRect(),
		_scissorRect(),
		_viewMat(),
		_projectionMat(),
		_viewPosition(),
		_globalUniformLayout(DefaultGlobalUniformLayout),
		_frustum(),
		_samples(),
		_renderTargets(),
		_meshDatas(),
		_renderPassShaderDatas(),
		_shaderDatas(),
		_materialDatas(),
		_objectDatas(),
		_directionalLightDatas(),
		_pointLightDatas()
	{}

	void RenderView::Setup(
		const RectInt& viewportRect, 
		const RectInt& scissorRect, 
		const Matrix4x4& viewMatrix, 
		const Matrix4x4& projectionMatrix,
		const Vector3& viewPosition,
		const ViewFrustum& frustum,
		MSAASamples samples, 
		const std::vector<RenderTarget>&renderTargets)
	{
		_viewportRect = viewportRect;
		_scissorRect = scissorRect;
		_viewMat = viewMatrix;
		_projectionMat = projectionMatrix;
		_viewPosition = viewPosition;
		_frustum = frustum;
		_samples = samples;
		_renderTargets = renderTargets;
	}

	void RenderView::Reset()
	{
		_renderTargets.clear();
		_globalUniformLayout = DefaultGlobalUniformLayout;
		_meshDatas.clear();
		_renderPassShaderDatas.clear();
		_shaderDatas.clear();
		_materialDatas.clear();
		_objectDatas.clear();
		_directionalLightDatas.clear();
		_pointLightDatas.clear();
	}

	RenderTarget& RenderView::GetRenderTarget(size_t index)
	{
		Assert(index < _renderTargets.size())

		return _renderTargets.at(index);
	}

	void RenderView::SetGlobalUniformLayout(const GlobalShaderUniformLayout& layout)
	{
		_globalUniformLayout = layout;
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
				_renderPassShaderDatas.try_emplace(passShaderID, passShaderID, shader.GetVersion(), passShader);

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
		std::any extraData)
	{
		SubMesh submesh;
		Assert(mesh.TryGetSubmesh(submeshID, submesh))
		return AddRenderObject(
			mesh,
			submesh.Offset, submesh.Count,
			modelMatrix,
			submesh.Bounds.Transformed(modelMatrix),
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
		std::any extraData)
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
			bounds,
			extraData);

		return objectID;
	}

	void RenderView::AddDirectionalLight(const Vector3& direction, const Color& color, double intensity)
	{
		Color c = color.AsLinear();
		c.A = intensity;
		_directionalLightDatas.emplace_back(direction, c);
	}

	void RenderView::AddPointLight(const Vector3& position, const Color& color, double intensity)
	{
		Color c = color.AsLinear();
		c.A = intensity;
		_pointLightDatas.emplace_back(position, c);
	}
}