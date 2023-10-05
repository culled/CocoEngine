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
		_shaderVariantDatas(),
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
		_shaderVariantDatas.clear();
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

			for (const ShaderVariant& variant : shader.GetShaderVariants())
			{
				uint64 variantID = variant.Hash;
				_shaderVariantDatas.try_emplace(variantID, variantID, shader.GetVersion(), variant);

				passShaders.try_emplace(variant.Name, variantID);
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

	const ShaderVariantData& RenderView::GetShaderVariantData(uint64 key) const
	{
		Assert(_shaderVariantDatas.contains(key))

		return _shaderVariantDatas.at(key);
	}

	uint64 RenderView::AddMaterial(const MaterialDataProvider& materialData)
	{
		uint64 id = materialData.GetMaterialID();

		if (!_materialDatas.contains(id))
		{
			SharedRef<Shader> shader = materialData.GetShader();
			uint64 shaderID = shader ? AddShader(*shader) : InvalidID;

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
		const MaterialDataProvider& material,
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
		uint32 submeshID, 
		const Matrix4x4& modelMatrix, 
		const Shader* shader, 
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
			shader,
			scissorRect,
			extraData);
	}

	uint64 RenderView::AddRenderObject(
		const Mesh& mesh, 
		uint64 indexOffset, 
		uint64 indexCount, 
		const Matrix4x4& modelMatrix,
		const BoundingBox& bounds,
		const MaterialDataProvider& material, 
		const RectInt* scissorRect,
		std::any extraData)
	{
		uint64 meshID = AddMesh(mesh);
		uint64 materialID = AddMaterial(material);
		uint64 shaderID = _materialDatas.at(materialID).ShaderID;

		uint64 objectID = _objectDatas.size();
		_objectDatas.emplace_back(
			objectID,
			modelMatrix,
			meshID,
			indexOffset,
			indexCount,
			materialID,
			shaderID,
			scissorRect ? *scissorRect : _scissorRect,
			bounds,
			extraData);

		return objectID;
	}

	uint64 RenderView::AddRenderObject(
		const Mesh& mesh, 
		uint64 indexOffset, 
		uint64 indexCount, 
		const Matrix4x4& modelMatrix, 
		const BoundingBox& bounds, 
		const Shader* shader, 
		const RectInt* scissorRect, 
		std::any extraData)
	{
		uint64 meshID = AddMesh(mesh);
		uint64 shaderID = shader ? AddShader(*shader) : InvalidID;

		uint64 objectID = _objectDatas.size();
		_objectDatas.emplace_back(
			objectID,
			modelMatrix,
			meshID,
			indexOffset,
			indexCount,
			InvalidID,
			shaderID,
			scissorRect ? *scissorRect : _scissorRect,
			bounds,
			extraData);

		return objectID;
	}

	const ObjectData& RenderView::GetRenderObject(uint64 index) const
	{
		Assert(index >= 0)
		Assert(index < _objectDatas.size())

		return _objectDatas.at(index);
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

	std::vector<uint64> RenderView::GetRenderObjectIndices() const
	{
		std::vector<uint64> indices(_objectDatas.size());

		for (uint64 i = 0; i < indices.size(); i++)
			indices.at(i) = i;

		return indices;
	}

	std::vector<ObjectData> RenderView::GetRenderObjects(std::span<const uint64> objectIndices) const
	{
		std::vector<ObjectData> objects;

		for (uint64 i = 0; i < objectIndices.size(); i++)
			objects.emplace_back(GetRenderObject(objectIndices[i]));

		return objects;
	}

	void RenderView::FilterOutsideFrustum(std::vector<uint64>& objectIndices) const
	{
		FilterOutsideFrustum(_frustum, objectIndices);
	}

	void RenderView::FilterOutsideFrustum(const ViewFrustum& frustum, std::vector<uint64>& objectIndices) const
	{
		auto it = objectIndices.begin();

		while (it != objectIndices.end())
		{
			const ObjectData& obj = GetRenderObject(*it);

			if (frustum.IsInside(obj.Bounds))
			{
				it++;
			}
			else
			{
				it = objectIndices.erase(it);
			}
		}
	}

	void RenderView::FilterWithTag(std::vector<uint64>& objectIndices, const string& tag) const
	{
		std::array<string, 1> tags = { tag };
		FilterWithTags(objectIndices, tags);
	}

	void RenderView::FilterWithTags(std::vector<uint64>& objectIndices, std::span<const string> tags) const
	{
		auto it = objectIndices.begin();

		while (it != objectIndices.end())
		{
			const ObjectData& obj = GetRenderObject(*it);

			if (obj.ShaderID == InvalidID)
			{
				it = objectIndices.erase(it);
				continue;
			}

			const ShaderData& shader = GetShaderData(obj.ShaderID);

			if (std::find(tags.begin(), tags.end(), shader.GroupTag) != tags.end())
			{
				it++;
			}
			else
			{
				it = objectIndices.erase(it);
			}
		}
	}

	void RenderView::FilterWithShaderVariant(std::vector<uint64>& objectIndices, const string& variantName) const
	{
		auto it = objectIndices.begin();

		while (it != objectIndices.end())
		{
			const ObjectData& obj = GetRenderObject(*it);

			if (obj.ShaderID == InvalidID)
			{
				it = objectIndices.erase(it);
				continue;
			}

			const ShaderData& shader = GetShaderData(obj.ShaderID);

			if (shader.Variants.contains(variantName))
			{
				it++;
			}
			else
			{
				it = objectIndices.erase(it);
			}
		}
	}
}