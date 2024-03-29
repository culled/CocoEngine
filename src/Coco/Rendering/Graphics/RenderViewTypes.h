#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/BoundingBox.h>
#include "../MeshTypes.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	class Image;
	class Buffer;

	/// @brief Sorting modes for render objects
	enum class RenderObjectSortMode
	{
		FrontToBack,
		BackToFront
	};

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

		/// @brief Sets clear values for all render targets using the clear color if it is a color target and the depth/stencil value otherwise
		/// @param renderTargets The render targets
		/// @param clearColor The color for clearing color targets
		/// @param clearDepth The depth value for clearing depth targets
		/// @param clearStencil The stencil value for clearing stencil targets
		static void SetClearValues(std::span<RenderTarget> renderTargets, const Color& clearColor, double clearDepth, uint8 clearStencil);

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

		/// @brief The format of this mesh data
		VertexDataFormat Format;

		/// @brief The buffer holding the vertex data
		Ref<Buffer> VertexBuffer;

		/// @brief The number of vertices in the vertex buffer
		uint64 VertexCount;

		/// @brief The buffer holding the index data
		Ref<Buffer> IndexBuffer;

		/// @brief The bounding box of this entire mesh
		BoundingBox Bounds;

		MeshData(
			uint64 id,
			uint64 version,
			const VertexDataFormat& format,
			const Ref<Buffer>& vertexBuffer,
			uint64 vertexCount,
			const Ref<Buffer>& indexBuffer,
			const BoundingBox& bounds);
	};

	/// @brief Data for a Material
	struct MaterialData
	{
		/// @brief The id of this material
		uint64 ID;

		/// @brief The uniform data for this material
		ShaderUniformData UniformData;

		MaterialData(uint64 id, const ShaderUniformData& uniformData);
	};

	/// @brief Data for an object to render
	struct ObjectData
	{
		/// @brief The render object ID
		uint64 ID;

		/// @brief The ID of the original object
		uint64 ObjectID;

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

		/// @brief The visibility groups for this object
		uint64 VisibilityGroups;

		/// @brief The scissor rectangle to use for rendering this object
		RectInt ScissorRect;

		/// @brief The bounding box of this object
		BoundingBox Bounds;

		/// @brief Extra stored data, if any
		std::any ExtraData;

		ObjectData(
			uint64 id,
			uint64 objectID,
			const Matrix4x4& modelMatrix,
			uint64 meshID,
			uint64 indexOffset,
			uint64 indexCount,
			uint64 materialID,
			uint64 visibilityGroups,
			const RectInt& scissorRect,
			const BoundingBox& bounds,
			std::any extraData);
	};

	/// @brief Data for a directional light
	struct DirectionalLightData
	{
		/// @brief The direction that the light is facing
		Vector3 Direction;

		/// @brief The color of the light, with the alpha value used for intensity
		Color Color;

		DirectionalLightData(const Vector3& direction, const Coco::Color& color);
	};

	/// @brief Data for a point light
	struct PointLightData
	{
		/// @brief The position of the light
		Vector3 Position;

		/// @brief The color of the light, with the alpha value used for intensity
		Color Color;

		PointLightData(const Vector3& position, const Coco::Color& color);
	};
}