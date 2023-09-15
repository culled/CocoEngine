#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>
#include "Image.h"
#include "Buffer.h"
#include "../Shader.h"

namespace Coco::Rendering
{
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

		/// @brief The buffer holding the vertex data
		Ref<Buffer> VertexBuffer;

		/// @brief The number of vertices in the vertex buffer
		uint32 VertexCount;

		/// @brief The buffer holding the index data
		Ref<Buffer> IndexBuffer;
		
		/// @brief The offset in the index buffer of the first index to draw
		uint32 FirstIndexOffset;

		/// @brief The number of indices to draw
		uint32 IndexCount;

		MeshData(
			const Ref<Buffer>& vertexBuffer,
			uint32 vertexCount,
			const Ref<Buffer>& indexBuffer,
			uint32 firstIndexOffset,
			uint32 indexCount);
	};

	struct RenderPassShaderData
	{
		uint64 ID;
		RenderPassShader ShaderData;
	};

	struct ShaderData
	{
		uint64 ID;
		string GroupTag;
		std::vector<uint64> RenderPassShaders;
	};

	/// @brief Holds information needed to render a scene
	class RenderView
	{
	private:
		RectInt _viewportRect;
		RectInt _scissorRect;
		std::vector<RenderTarget> _renderTargets;
		std::unordered_map<uint64, MeshData> _meshDatas;
		std::unordered_map<uint64, RenderPassShaderData> _renderPassShaderDatas;
		std::unordered_map<uint64, ShaderData> _shaderDatas;

	public:
		RenderView(
			const RectInt& viewportRect, 
			const RectInt& scissorRect, 
			const std::vector<RenderTarget>& renderTargets);

		/// @brief Gets the viewport rectangle
		/// @return The viewport rectangle
		const RectInt& GetViewportRect() const { return _viewportRect; }

		/// @brief Gets the scissor rectangle (actual part of viewport that is rendered)
		/// @return The scissor rectangle
		const RectInt& GetScissorRect() const { return _scissorRect; }

		/// @brief Gets the render targets
		/// @return The render targets
		const std::vector<RenderTarget>& GetRenderTargets() const { return _renderTargets; }

		/// @brief Gets a single render target
		/// @param index The index of the render target
		/// @return The render target at the given index
		RenderTarget& GetRenderTarget(size_t index);

		const MeshData& GetMeshData(uint64 key) const;
	};
}