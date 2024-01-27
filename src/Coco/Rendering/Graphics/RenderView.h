#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include <Coco/Core/Resources/ResourceTypes.h>
#include "RenderViewTypes.h"
#include "ShaderUniformLayout.h"
#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	class RenderFrame;
	class Mesh;
	class Material;
	struct Submesh;

	/// @brief Holds data for a render from a specific point-of-view
	class RenderView
	{
	public:
		/// @brief The default layout for global uniforms
		static const ShaderUniformLayout DefaultGlobalUniformLayout;

	public:
		RenderView(RenderFrame& renderFrame);
		~RenderView();

		void Setup(
			std::span<RenderTarget> renderTargets,
			const RectInt& viewportRect,
			const Matrix4x4& viewMatrix,
			const Matrix4x4& projectionMatrix,
			const ViewFrustum& frustum,
			MSAASamples samples,
			std::optional<ShaderUniformLayout> globalUniformLayout = std::optional<ShaderUniformLayout>());

		void SetGlobalUniformValues(std::span<const ShaderUniformValue> uniforms);
		std::span< const ShaderUniformValue> GetGlobalUniformValues() const { return _globalUniformValues; }

		const ShaderUniformLayout& GetGlobalUniformLayout() const { return _globalUniformLayout; }

		const RectInt& GetViewportRect() const { return _viewportRect; }
		MSAASamples GetSampleCount() const { return _samples; }
		std::span<const RenderTarget> GetRenderTargets() const { return _renderTargets; }
		const ViewFrustum& GetViewFrustum() const { return _frustum; }

		RenderObjectData& AddRenderObject(
			uint64 objectID,
			uint64 visibilityGroups,
			SharedRef<Mesh> mesh,
			const Submesh& submesh,
			SharedRef<Material> material,
			const Matrix4x4& modelMatrix);

		std::span<const RenderObjectData> GetRenderObjects() const { return _renderObjects; }

		/// @brief Gets a render object at the given index
		/// @param index The index of the object
		/// @return The object
		const RenderObjectData& GetRenderObject(uint64 index) const;

		/// @brief Gets a vector of all render object indices
		/// @return A vector of indices for every render object
		std::vector<uint64> GetRenderObjectIndices() const;

		/// @brief Filters out object indices that are outside of this view's frustum
		/// @param objectIndices The object indices
		void FilterOutsideFrustum(std::vector<uint64>& objectIndices) const;

		/// @brief Filters out object indices that are outside of a frustum
		/// @param objectIndices The object indices
		/// @param frustum The view frustum
		void FilterOutsideFrustum(std::vector<uint64>& objectIndices, const ViewFrustum& frustum) const;

		/// @brief Filters out object indicies that do not include any of the given visibility groups
		/// @param objectIndices The object indices
		/// @param visibilityGroups The visibility groups
		void FilterWithAnyVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const;

		/// @brief Filters out object indicies that do not include all of the given visibility groups
		/// @param objectIndices The object indices
		/// @param visibilityGroups The visibility groups
		void FilterWithAllVisibilityGroups(std::vector<uint64>& objectIndices, uint64 visibilityGroups) const;

		/// @brief Sorts objects by distance from this view's view position
		/// @param objectIndices The object indices
		/// @param sortMode The sort mode
		void SortByDistance(std::vector<uint64>& objectIndices, RenderObjectSortMode sortMode) const;

		/// @brief Sorts objects by distance from a view position 
		/// @param objectIndices The object indices
		/// @param position The position to sort relative to
		/// @param sortMode The sort mode
		void SortByDistance(std::vector<uint64>& objectIndices, const Vector3& position, RenderObjectSortMode sortMode) const;

	private:
		RenderFrame& _renderFrame;
		ShaderUniformLayout _globalUniformLayout;
		std::vector<ShaderUniformValue> _globalUniformValues;
		RectInt _viewportRect;
		Matrix4x4 _viewMat;
		Matrix4x4 _projectionMat;
		ViewFrustum _frustum;
		MSAASamples _samples;
		std::vector<RenderTarget> _renderTargets;

		std::unordered_map<ResourceID, SharedRef<Mesh>> _meshes;
		std::vector<RenderObjectData> _renderObjects;

	private:
		void StageMeshData(const SharedRef<Mesh>& mesh);
	};
}