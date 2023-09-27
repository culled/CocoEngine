#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
#include "RenderPassShader.h"

namespace Coco::Rendering
{
	class Mesh;
	class Shader;
	class MaterialDataProvider;
	class Image;
	class Buffer;

	/// @brief An image that can be rendered to
	struct RenderTarget
	{
		/// @brief The actual image that will be rendered to
		Ref<Image> MainImage;

		/// @brief If using MSAA, this is the image that will be resolved to from the main image
		Ref<Image> ResolveImage;

		/// @brief Clear values for clearing the image (if it needs to be cleared)
		Vector4 ClearValue;

		RenderTarget();
		RenderTarget(Ref<Image> mainImage, const Vector4& clearValue);

		/// @brief Sets the clear value from a color
		/// @param clearColor The clear color
		void SetColorClearValue(const Color& clearColor);

		/// @brief Sets the clear value for a depth image
		/// @param depthClearValue The depth to clear to
		void SetDepthClearValue(double depthClearValue);

		/// @brief Sets the clear values for a depth/stencil image
		/// @param depthClearValue The depth to clear to
		/// @param stencilClearValue The value to clear the stencil to
		void SetDepthStencilClearValue(double depthClearValue, uint8 stencilClearValue);
	};

	/// @brief Data for a Mesh
	struct MeshData
	{
		/// @brief The ID of this data
		uint64 ID;

		/// @brief The version of this data
		uint64 Version;

		/// @brief The buffer holding the vertex data
		Ref<Buffer> VertexBuffer;

		/// @brief The number of vertices in the vertex buffer
		uint64 VertexCount;

		/// @brief The buffer holding the index data
		Ref<Buffer> IndexBuffer;

		MeshData(
			uint64 id,
			uint64 version,
			const Ref<Buffer>& vertexBuffer,
			uint64 vertexCount,
			const Ref<Buffer>& indexBuffer);
	};

	/// @brief Data for a single RenderPassShader
	struct RenderPassShaderData
	{
		/// @brief The id of this shader
		uint64 ID;

		/// @brief The version of this data
		uint64 Version;

		/// @brief The shader data
		RenderPassShader ShaderData;

		RenderPassShaderData(uint64 id, uint64 version, const RenderPassShader& shaderData);
	};

	/// @brief Data for a Shader
	struct ShaderData
	{
		/// @brief The id of this shader
		uint64 ID;

		/// @brief The version of this shader
		uint64 Version;

		/// @brief The group of this shader
		string GroupTag;

		/// @brief Renderpass shaders for this shader
		std::unordered_map<string, uint64> RenderPassShaders;

		ShaderData(uint64 id, uint64 version, const string& groupTag, const std::unordered_map<string, uint64>& passShaders);
	};

	/// @brief Data for a Material
	struct MaterialData
	{
		/// @brief The id of this material
		uint64 ID;

		/// @brief The id of the shader that this material uses
		uint64 ShaderID;

		/// @brief The uniform data for this material
		ShaderUniformData UniformData;

		MaterialData(uint64 id, uint64 shaderID, const ShaderUniformData& uniformData);
	};

	/// @brief Data for an object to render
	struct ObjectData
	{
		/// @brief The object ID
		uint64 ID;

		/// @brief The model matrix to render with
		Matrix4x4 ModelMatrix;

		/// @brief The ID of the mesh to render with
		uint64 MeshID;

		/// @brief The offset of the first index to render
		uint64 IndexOffset;

		/// @brief The number of indices to render
		uint64 IndexCount;

		/// @brief The ID of the material to render with
		uint64 MaterialID;

		/// @brief The scissor rectangle to use for rendering this object
		RectInt ScissorRect;

		ObjectData(
			uint64 id, 
			const Matrix4x4& modelMatrix, 
			uint64 meshID, 
			uint64 indexOffset,
			uint64 indexCount,
			uint64 materialID, 
			const RectInt& scissorRect);
	};

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
		MSAASamples _samples;
		std::vector<RenderTarget> _renderTargets;
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
		/// @param samples The number of MSAA samples
		/// @param renderTargets The render targets
		void Setup(const RectInt& viewportRect,
			const RectInt& scissorRect,
			const Matrix4x4& viewMatrix,
			const Matrix4x4& projectionMatrix,
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
		/// @return The key to the object
		uint64 AddRenderObject(
			const Mesh& mesh, 
			uint32 submeshID, 
			const Matrix4x4& modelMatrix, 
			const MaterialDataProvider* material, 
			const RectInt* scissorRect = nullptr);

		/// @brief Adds an object to be rendered. 
		/// NOTE: the mesh must have been applied for this to work
		/// @param mesh The mesh
		/// @param indexOffset The offset of the first index in the mesh's index buffer
		/// @param indexCount The number of indices to render
		/// @param modelMatrix The model matrix
		/// @param material The material, or nullptr for no material
		/// @param scissorRect The scissor rect, or nullptr for no scissor rectangle
		/// @return The key to the object
		uint64 AddRenderObject(
			const Mesh& mesh,
			uint64 indexOffset,
			uint64 indexCount,
			const Matrix4x4& modelMatrix,
			const MaterialDataProvider* material,
			const RectInt* scissorRect = nullptr);

		/// @brief Gets all objects to be rendered
		/// @return The renderable objects
		std::span<const ObjectData> GetRenderObjects() const { return _objectDatas; }
	};
}