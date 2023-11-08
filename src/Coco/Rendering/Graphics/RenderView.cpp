#include "Renderpch.h"
#include "RenderView.h"
#include "../Mesh.h"
#include "../Material.h"
#include "ShaderUniformLayout.h"
#include "../RenderService.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	const GlobalShaderUniformLayout RenderView::DefaultGlobalUniformLayout = GlobalShaderUniformLayout(
		{
			ShaderUniform("ProjectionMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity),
			ShaderUniform("ViewMatrix", ShaderUniformType::Mat4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
		},
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
				mesh.GetVertexFormat(),
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

	uint64 RenderView::AddMaterial(const MaterialDataProvider& materialData)
	{
		uint64 id = materialData.GetMaterialID();

		if (!_materialDatas.contains(id))
		{
			_materialDatas.try_emplace(id, id, materialData.GetUniformData());
		}

		return id;
	}

	const MaterialData& RenderView::GetMaterialData(uint64 key) const
	{
		Assert(_materialDatas.contains(key))

		return _materialDatas.at(key);
	}

	uint64 RenderView::AddRenderObject(
		uint64 objectID,
		const Mesh& mesh, 
		uint32 submeshID, 
		const Matrix4x4& modelMatrix,
		uint64 visibilityGroups,
		const MaterialDataProvider* material,
		const BoundingBox* boundsOverride,
		const RectInt* scissorRect,
		std::any extraData)
	{
		SubMesh submesh;
		Assert(mesh.TryGetSubmesh(submeshID, submesh))
		return AddRenderObject(
			objectID,
			mesh,
			submesh.Offset, submesh.Count,
			modelMatrix,
			boundsOverride ? *boundsOverride : submesh.Bounds.Transformed(modelMatrix),
			visibilityGroups,
			material,
			scissorRect,
			extraData);
	}

	uint64 RenderView::AddRenderObject(
		uint64 objectID,
		const Mesh& mesh, 
		uint64 indexOffset, 
		uint64 indexCount, 
		const Matrix4x4& modelMatrix,
		const BoundingBox& bounds,
		uint64 visibilityGroups,
		const MaterialDataProvider* material, 
		const RectInt* scissorRect,
		std::any extraData)
	{
		uint64 meshID = AddMesh(mesh);
		uint64 materialID = material ? AddMaterial(*material) : InvalidID;

		uint64 renderID = _objectDatas.size();
		_objectDatas.emplace_back(
			renderID,
			objectID,
			modelMatrix,
			meshID,
			indexOffset,
			indexCount,
			materialID,
			visibilityGroups,
			scissorRect ? *scissorRect : _scissorRect,
			bounds,
			extraData);

		return renderID;
	}

	const ObjectData& RenderView::GetRenderObject(uint64 index) const
	{
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
		FilterOutsideFrustum(objectIndices, _frustum);
	}

	void RenderView::FilterOutsideFrustum(std::vector<uint64>& objectIndices, const ViewFrustum& frustum) const
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

	void RenderView::FilterWithAnyVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const
	{
		auto it = objectIndices.begin();

		while (it != objectIndices.end())
		{
			const ObjectData& obj = GetRenderObject(*it);

			if ((obj.VisibilityGroups & visibilityGroups) != 0)
			{
				it++;
			}
			else
			{
				it = objectIndices.erase(it);
			}
		}
	}

	void RenderView::FilterWithAllVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const
	{
		auto it = objectIndices.begin();

		while (it != objectIndices.end())
		{
			const ObjectData& obj = GetRenderObject(*it);

			if ((obj.VisibilityGroups & visibilityGroups) == visibilityGroups)
			{
				it++;
			}
			else
			{
				it = objectIndices.erase(it);
			}
		}
	}

	void RenderView::SortByDistance(std::vector<uint64>& objectIndices, RenderObjectSortMode sortMode) const
	{
		SortByDistance(objectIndices, _viewPosition, sortMode);
	}

	void RenderView::SortByDistance(std::vector<uint64>& objectIndices, const Vector3& position, RenderObjectSortMode sortMode) const
	{
		std::sort(objectIndices.begin(), objectIndices.end(), [&, position, sortMode](const uint64& a, const uint64& b)
			{
				const ObjectData& objA = GetRenderObject(a);
				const ObjectData& objB = GetRenderObject(b);

				double distA = Vector3::DistanceBetween(position, objA.Bounds.GetCenter());
				double distB = Vector3::DistanceBetween(position, objB.Bounds.GetCenter());

				switch (sortMode)
				{
				case RenderObjectSortMode::BackToFront:
					return distA > distB;
				case RenderObjectSortMode::FrontToBack:
					return distA < distB;
				default:
					return true;
				}
			});
	}
}