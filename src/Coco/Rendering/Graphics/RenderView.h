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
		static constexpr uint64 InvalidID = Math::MaxValue<uint64>();

	private:
		RectInt _viewportRect;
		RectInt _scissorRect;
		Matrix4x4 _viewMat;
		Matrix4x4 _projectionMat;
		ViewFrustum _frustum;
		MSAASamples _samples;
		std::vector<RenderTarget> _renderTargets;
		std::optional<GlobalShaderUniformLayout> _globalUniformLayout;
		std::unordered_map<uint64, MeshData> _meshDatas;
		std::unordered_map<uint64, RenderPassShaderData> _renderPassShaderDatas;
		std::unordered_map<uint64, ShaderData> _shaderDatas;
		std::unordered_map<uint64, MaterialData> _materialDatas;
		std::vector<ObjectData> _objectDatas;

	public:
		RenderView();

		/// @brief Sets up this RenderView for a new render
		/// @param viewportRect The viewport rectangle
		/// @param scissorRect The scissor rectangle
		/// @param viewMatrix The view matrix
		/// @param projectionMatrix The projection matrix
		/// @param frustum The view frustum
		/// @param samples The number of MSAA samples
		/// @param renderTargets The render targets
		void Setup(const RectInt& viewportRect,
			const RectInt& scissorRect,
			const Matrix4x4& viewMatrix,
			const Matrix4x4& projectionMatrix,
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

		/// @brief Determines if a global uniform layout has been set
		/// @return True if a global uniform layout has been set
		bool HasGlobalUniformLayout() const { return _globalUniformLayout.has_value(); }

		/// @brief Gets the global uniform layout.
		/// NOTE: check if the layout exists first by calling HasGlobalUniformLayout()
		/// @return The global uniform layout
		const GlobalShaderUniformLayout& GetGlobalUniformLayout() const { return _globalUniformLayout.value(); }

		/// @brief Adds a mesh to this view
		/// @param mesh The mesh
		/// @return The id of the stored mesh
		uint64 AddMesh(const Mesh& mesh);

		/// @brief Gets a stored mesh's data
		/// @param key The mesh's key
		/// @return The data
		const MeshData& GetMeshData(uint64 key) const;

		/// @brief Adds a shader to this view
		/// @param shader The shader
		/// @return The id of the stored shader
		uint64 AddShader(const Shader& shader);

		/// @brief Gets a stored shader's data
		/// @param key The shader's key
		/// @return The shader data
		const ShaderData& GetShaderData(uint64 key) const;

		/// @brief Gets a stored render pass shader's data
		/// @param key The shader's key
		/// @return The render pass shader data
		const RenderPassShaderData& GetRenderPassShaderData(uint64 key) const;

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
		/// @param mesh The mesh
		/// @param submeshID The ID of the submesh
		/// @param modelMatrix The model matrix
		/// @param material The material, or nullptr for no material
		/// @param scissorRect The scissor rect, or nullptr for no scissor rectangle
		/// @param extraData Extra data for the object
		/// @return The key to the object
		uint64 AddRenderObject(
			const Mesh& mesh, 
			uint32 submeshID, 
			const Matrix4x4& modelMatrix, 
			const MaterialDataProvider* material, 
			const RectInt* scissorRect = nullptr,
			SharedRef<ExtraObjectData> extraData = nullptr);

		/// @brief Adds an object to be rendered. 
		/// NOTE: the mesh must have been applied for this to work
		/// @param mesh The mesh
		/// @param indexOffset The offset of the first index in the mesh's index buffer
		/// @param indexCount The number of indices to render
		/// @param modelMatrix The model matrix
		/// @param bounds The bounds of the object
		/// @param material The material, or nullptr for no material
		/// @param scissorRect The scissor rect, or nullptr for no scissor rectangle
		/// @param extraData Extra data for the object
		/// @return The key to the object
		uint64 AddRenderObject(
			const Mesh& mesh,
			uint64 indexOffset,
			uint64 indexCount,
			const Matrix4x4& modelMatrix,
			const BoundingBox& bounds,
			const MaterialDataProvider* material,
			const RectInt* scissorRect = nullptr,
			SharedRef<ExtraObjectData> extraData = nullptr);

		/// @brief Gets all objects to be rendered
		/// @return The renderable objects
		std::span<const ObjectData> GetRenderObjects() const { return _objectDatas; }
	};
}