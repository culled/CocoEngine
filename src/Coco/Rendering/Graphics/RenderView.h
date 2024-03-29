#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include "ShaderUniformLayout.h"
#include "RenderViewTypes.h"

namespace Coco::Rendering
{
	class Mesh;
	class Shader;
	class MaterialDataProvider;

	/// @brief Holds information needed to render a scene
	class RenderView
	{
	public:
		/// @brief An invalid resource ID
		static constexpr uint64 InvalidID = Math::MaxValue<uint64>();

		/// @brief The default layout for global uniforms
		static const GlobalShaderUniformLayout DefaultGlobalUniformLayout;

	private:
		RectInt _viewportRect;
		RectInt _scissorRect;
		Matrix4x4 _viewMat;
		Matrix4x4 _projectionMat;
		Vector3 _viewPosition;
		ViewFrustum _frustum;
		MSAASamples _samples;
		std::vector<RenderTarget> _renderTargets;
		GlobalShaderUniformLayout _globalUniformLayout;
		std::unordered_map<uint64, MeshData> _meshDatas;
		std::unordered_map<uint64, MaterialData> _materialDatas;
		std::vector<ObjectData> _objectDatas;
		std::vector<DirectionalLightData> _directionalLightDatas;
		std::vector<PointLightData> _pointLightDatas;

	public:
		RenderView();

		/// @brief Sets up this RenderView for a new render
		/// @param viewportRect The viewport rectangle
		/// @param scissorRect The scissor rectangle
		/// @param viewMatrix The view matrix
		/// @param projectionMatrix The projection matrix
		/// @param viewPosition The view position
		/// @param frustum The view frustum
		/// @param samples The number of MSAA samples
		/// @param renderTargets The render targets
		void Setup(const RectInt& viewportRect,
			const RectInt& scissorRect,
			const Matrix4x4& viewMatrix,
			const Matrix4x4& projectionMatrix,
			const Vector3& viewPosition,
			const ViewFrustum& frustum,
			MSAASamples samples,
			const std::vector<RenderTarget>& renderTargets);

		/// @brief Resets this RenderView
		void Reset();

		/// @brief Gets the viewport rectangle
		/// @return The viewport rectangle
		const RectInt& GetViewportRect() const { return _viewportRect; }

		/// @brief Gets the scissor rectangle (actual part of viewport that is rendered)
		/// @return The scissor rectangle
		const RectInt& GetScissorRect() const { return _scissorRect; }

		/// @brief Gets the view matrix
		/// @return The view matrix
		const Matrix4x4& GetViewMatrix() const { return _viewMat; }

		/// @brief Gets the projection matrix
		/// @return The projection matrix
		const Matrix4x4& GetProjectionMatrix() const { return _projectionMat; }

		/// @brief Gets the view position
		/// @return The view position
		const Vector3& GetViewPosition() const { return _viewPosition; }

		/// @brief Gets the view frustum
		/// @return The view frustum
		const ViewFrustum& GetViewFrustum() const { return _frustum; }

		/// @brief Gets the number of MSAA samples that should be used for rendering
		/// @return The MSAA samples
		MSAASamples GetMSAASamples() const { return _samples; }

		/// @brief Gets the render targets
		/// @return The render targets
		std::span<RenderTarget> GetRenderTargets() { return _renderTargets; }

		/// @brief Gets the render targets
		/// @return The render targets
		std::span<const RenderTarget> GetRenderTargets() const { return _renderTargets; }

		/// @brief Gets a single render target
		/// @param index The index of the render target
		/// @return The render target at the given index
		RenderTarget& GetRenderTarget(size_t index);
		
		/// @brief Sets the layout of the global uniforms
		/// @param layout The global uniform layout
		void SetGlobalUniformLayout(const GlobalShaderUniformLayout& layout);

		/// @brief Gets the global uniform layout
		/// @return The global uniform layout
		const GlobalShaderUniformLayout& GetGlobalUniformLayout() const { return _globalUniformLayout; }

		/// @brief Adds a mesh to this view
		/// @param mesh The mesh
		/// @return The id of the stored mesh
		uint64 AddMesh(const Mesh& mesh);

		/// @brief Gets a stored mesh's data
		/// @param key The mesh's key
		/// @return The data
		const MeshData& GetMeshData(uint64 key) const;

		/// @brief Adds material data to this view
		/// @param materialData The material data
		/// @return The key to the material data
		uint64 AddMaterial(const MaterialDataProvider& materialData);

		/// @brief Gets stored material data
		/// @param key The data's key
		/// @return The material data
		const MaterialData& GetMaterialData(uint64 key) const;

		/// @brief Adds an object to be rendered. 
		/// NOTE: the mesh must have been applied for this to work
		/// @param objectID The id of the object
		/// @param mesh The mesh
		/// @param submeshID The ID of the submesh
		/// @param modelMatrix The model matrix
		/// @param visibilityGroups The visibility groups for the object
		/// @param material The material, or nullptr for no material
		/// @param boundsOverride The global bounding box for the object, or nullptr to auto-calculate the global bounding box
		/// @param scissorRect The scissor rect, or nullptr for no scissor rectangle
		/// @param extraData Extra data for the object
		/// @return The key to the object
		uint64 AddRenderObject(
			uint64 objectID,
			const Mesh& mesh, 
			uint32 submeshID, 
			const Matrix4x4& modelMatrix,
			uint64 visibilityGroups,
			const MaterialDataProvider* material, 
			const BoundingBox* boundsOverride = nullptr,
			const RectInt* scissorRect = nullptr,
			std::any extraData = nullptr);

		/// @brief Adds an object to be rendered. 
		/// NOTE: the mesh must have been applied for this to work
		/// @param objectID The id of the object
		/// @param mesh The mesh
		/// @param indexOffset The offset of the first index in the mesh's index buffer
		/// @param indexCount The number of indices to render
		/// @param modelMatrix The model matrix
		/// @param bounds The bounds of the object
		/// @param visibilityGroups The visibility groups for the object
		/// @param material The material, or nullptr for no material
		/// @param scissorRect The scissor rect, or nullptr for no scissor rectangle
		/// @param extraData Extra data for the object
		/// @return The key to the object
		uint64 AddRenderObject(
			uint64 objectID,
			const Mesh& mesh,
			uint64 indexOffset,
			uint64 indexCount,
			const Matrix4x4& modelMatrix,
			const BoundingBox& bounds,
			uint64 visibilityGroups,
			const MaterialDataProvider* material,
			const RectInt* scissorRect = nullptr,
			std::any extraData = nullptr);

		/// @brief Gets all objects to be rendered
		/// @return The renderable objects
		const std::vector<ObjectData>& GetRenderObjects() const { return _objectDatas; }

		/// @brief Gets a render object at the given index
		/// @param index The index of the object
		/// @return The object
		const ObjectData& GetRenderObject(uint64 index) const;

		/// @brief Adds a directional light
		/// @param direction The direction that the light is facing
		/// @param color The color of the light
		/// @param intensity The intensity of the light
		void AddDirectionalLight(const Vector3& direction, const Color& color, double intensity);

		/// @brief Gets all the directional lights
		/// @return The directional lights
		std::span<const DirectionalLightData> GetDirectionalLights() const { return _directionalLightDatas; }

		/// @brief Adds a point light
		/// @param position The position of the light
		/// @param color The color of the light
		/// @param intensity The intensity of the light
		void AddPointLight(const Vector3& position, const Color& color, double intensity);

		/// @brief Gets all the point lights
		/// @return The point lights
		std::span<const PointLightData> GetPointLights() const { return _pointLightDatas; }

		/// @brief Gets a vector of all render object indices
		/// @return A vector of indices for every render object
		std::vector<uint64> GetRenderObjectIndices() const;

		/// @brief Gets objects according to the given object indices
		/// @param objectIndices The indices of the objects to get
		/// @return The render objects
		std::vector<ObjectData> GetRenderObjects(std::span<const uint64> objectIndices) const;

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
	};
}