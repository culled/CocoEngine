#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
#include "Image.h"
#include "Buffer.h"
#include "RenderPassShader.h"

namespace Coco::Rendering
{
	class Mesh;
	class Shader;

	/// @brief An image that can be rendered to
	struct RenderTarget
	{
		/// @brief The actual image that will be rendered to
		Ref<Image> Image;

		/// @brief Clear values for clearing the image the first time it is used (if it needs to be cleared)
		Vector4 ClearValue;

		RenderTarget(const Ref<Rendering::Image>& image);
		RenderTarget(const Ref<Rendering::Image>& image, Vector4 clearValue);
		RenderTarget(const Ref<Rendering::Image>& image, Color clearColor);
		RenderTarget(const Ref<Rendering::Image>& image, double depthClearValue);
		RenderTarget(const Ref<Rendering::Image>& image, Vector2 depthStencilClearValue);
	};

	/// @brief Data for a single mesh
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
		
		/// @brief The offset in the index buffer of the first index to draw
		uint64 FirstIndexOffset;

		/// @brief The number of indices to draw
		uint64 IndexCount;

		MeshData(
			uint64 id,
			uint64 version,
			const Ref<Buffer>& vertexBuffer,
			uint64 vertexCount,
			const Ref<Buffer>& indexBuffer,
			uint64 firstIndexOffset,
			uint64 indexCount);
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

	/// @brief Data for a shader
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

	/// @brief Data for an object to render
	struct ObjectData
	{
		/// @brief The object ID
		uint64 ID;

		/// @brief The model matrix to render with
		Matrix4x4 ModelMatrix;

		/// @brief The ID of the mesh to render with
		uint64 MeshID;

		// HACK: temporary
		uint64 ShaderID;

		ObjectData(uint64 id, const Matrix4x4& modelMatrix, uint64 meshID, uint64 shaderID);
	};

	/// @brief Holds information needed to render a scene
	class RenderView
	{
	private:
		RectInt _viewportRect;
		RectInt _scissorRect;
		Matrix4x4 _viewMat;
		Matrix4x4 _projectionMat;
		std::vector<RenderTarget> _renderTargets;
		std::unordered_map<uint64, MeshData> _meshDatas;
		std::unordered_map<uint64, RenderPassShaderData> _renderPassShaderDatas;
		std::unordered_map<uint64, ShaderData> _shaderDatas;
		std::vector<ObjectData> _objectDatas;

	public:
		RenderView(
			const RectInt& viewportRect, 
			const RectInt& scissorRect, 
			const Matrix4x4& viewMatrix,
			const Matrix4x4& projectionMatrix,
			const std::vector<RenderTarget>& renderTargets);

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
		/// @param submeshID The id of the submesh
		/// @return The id of the stored mesh
		uint64 AddMesh(const Mesh& mesh, uint32 submeshID);

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

		/// @brief Adds an object to be rendered
		/// @param mesh The mesh
		/// @param submeshID The ID of the submesh
		/// @param shader The shader
		/// @param modelMatrix The model matrix
		void AddRenderObject(const Mesh& mesh, uint32 submeshID, const Shader& shader, const Matrix4x4& modelMatrix);

		/// @brief Gets all objects to be rendered
		/// @return The renderable objects
		std::span<const ObjectData> GetRenderObjects() const { return _objectDatas; }
	};
}