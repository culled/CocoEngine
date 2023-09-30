#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Rect.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/BoundingBox.h>
#include "RenderPassShader.h"

namespace Coco::Rendering
{
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

		/// @brief The bounding box of this entire mesh
		BoundingBox Bounds;

		MeshData(
			uint64 id,
			uint64 version,
			const Ref<Buffer>& vertexBuffer,
			uint64 vertexCount,
			const Ref<Buffer>& indexBuffer,
			const BoundingBox& bounds);
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

		/// @brief The bounding box of this object
		BoundingBox Bounds;

		/// @brief Extra stored data, if any
		std::any ExtraData;

		ObjectData(
			uint64 id,
			const Matrix4x4& modelMatrix,
			uint64 meshID,
			uint64 indexOffset,
			uint64 indexCount,
			uint64 materialID,
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