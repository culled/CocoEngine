#include "Renderpch.h"
#include "RenderView.h"
#include "RenderFrame.h"
#include "../RenderService.h"
#include "../Mesh.h"

namespace Coco::Rendering
{
	const ShaderUniformLayout RenderView::DefaultGlobalUniformLayout = ShaderUniformLayout(
		{
			ShaderUniform("ProjectionMatrix", ShaderUniformType::Matrix4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity),
			ShaderUniform("ViewMatrix", ShaderUniformType::Matrix4x4, ShaderStageFlags::Vertex, Matrix4x4::Identity)
		}
	);

	RenderView::RenderView(RenderFrame& renderFrame) :
		_renderFrame(renderFrame),
		_globalUniformLayout(DefaultGlobalUniformLayout),
		_globalUniformValues(),
		_viewportRect(),
		_viewMat(),
		_projectionMat(),
		_frustum(),
		_samples(MSAASamples::One),
		_renderTargets(),
		_meshes(),
		_renderObjects()
	{}

	void RenderView::Setup(
		std::span<RenderTarget> renderTargets, 
		const RectInt& viewportRect, 
		const Matrix4x4& viewMatrix, 
		const Matrix4x4& projectionMatrix, 
		const ViewFrustum& frustum, 
		MSAASamples samples, 
		std::optional<ShaderUniformLayout> globalUniformLayout)
	{
		_viewportRect = viewportRect;
		_viewMat = viewMatrix;
		_projectionMat = projectionMatrix;
		_frustum = frustum;
		_samples = samples;
		_renderTargets = std::vector<RenderTarget>(renderTargets.begin(), renderTargets.end());

		if (globalUniformLayout.has_value())
		{
			_globalUniformLayout = globalUniformLayout.value();
		}
		else
		{
			_globalUniformLayout = DefaultGlobalUniformLayout;
			_globalUniformValues.emplace_back("ProjectionMatrix", projectionMatrix);
			_globalUniformValues.emplace_back("ViewMatrix", viewMatrix);
		}

		if (_globalUniformLayout.NeedsDataCalculation())
		{
			RenderService* rendering = RenderService::Get();
			_globalUniformLayout.CalculateDataUniforms(rendering->GetDevice());
		}
	}

	void RenderView::SetGlobalUniformValues(std::span<const ShaderUniformValue> uniforms)
	{
		_globalUniformValues = std::vector<ShaderUniformValue>(uniforms.begin(), uniforms.end());
	}

	RenderObjectData& RenderView::AddRenderObject(
		uint64 objectID,
		uint64 visibilityGroups, 
		SharedRef<Mesh> mesh,
		const Submesh& submesh,
		SharedRef<Material> material,
		const Matrix4x4& modelMatrix)
	{
		BoundingBox bounds;

		if (mesh)
		{
			StageMeshData(mesh);
			bounds = mesh->GetLocalBounds().Transformed(modelMatrix);
		}

		return _renderObjects.emplace_back(
			objectID,
			visibilityGroups,
			mesh,
			submesh,
			material,
			modelMatrix,
			bounds,
			nullptr
		);
	}

	const RenderObjectData& RenderView::GetRenderObject(uint64 index) const
	{
		return _renderObjects.at(index);
	}

	std::vector<uint64> RenderView::GetRenderObjectIndices() const
	{
		std::vector<uint64> indices;

		for (uint64 i = 0; i < _renderObjects.size(); i++)
			indices.emplace_back(i);

		return indices;
	}

	void RenderView::FilterOutsideFrustum(std::vector<uint64>& objectIndices) const
	{
		FilterOutsideFrustum(objectIndices, _frustum);
	}

	void RenderView::FilterOutsideFrustum(std::vector<uint64>& objectIndices, const ViewFrustum& frustum) const
	{
		std::erase_if(objectIndices,
			[&, frustum](const uint64& index)
			{
				const RenderObjectData& obj = GetRenderObject(index);

				return !frustum.IsInside(obj.Bounds);
			});
	}

	void RenderView::FilterWithAnyVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const
	{
		std::erase_if(objectIndices,
			[&, visibilityGroups](const uint64& index)
			{
				const RenderObjectData& obj = GetRenderObject(index);

				return (obj.VisibilityGroups & visibilityGroups) == 0;
			});
	}

	void RenderView::FilterWithAllVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const
	{
		std::erase_if(objectIndices,
			[&, visibilityGroups](const uint64& index)
			{
				const RenderObjectData& obj = GetRenderObject(index);

				return (obj.VisibilityGroups & visibilityGroups) != visibilityGroups;
			});
	}

	void RenderView::SortByDistance(std::vector<uint64>& objectIndices, RenderObjectSortMode sortMode) const
	{
		Vector3 viewPosition = _viewMat.Inverted().GetTranslation();
		SortByDistance(objectIndices, viewPosition, sortMode);
	}

	void RenderView::SortByDistance(std::vector<uint64>& objectIndices, const Vector3& position, RenderObjectSortMode sortMode) const
	{
		std::sort(objectIndices.begin(), objectIndices.end(), [&, position, sortMode](const uint64& a, const uint64& b)
			{
				const RenderObjectData& objA = GetRenderObject(a);
				const RenderObjectData& objB = GetRenderObject(b);

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

	void RenderView::StageMeshData(const SharedRef<Mesh>& mesh)
	{
		if (!mesh)
			return;

		const ResourceID& meshID = mesh->GetID();
		auto result = _meshes.try_emplace(meshID, mesh);

		if (!result.second || !mesh->NeedsUpload())
			return;

		_renderFrame.UploadMesh(mesh);
	}
}