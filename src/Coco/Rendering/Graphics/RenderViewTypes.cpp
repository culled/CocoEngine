#include "Renderpch.h"
#include "RenderViewTypes.h"
#include "Image.h"
#include "Buffer.h"

namespace Coco::Rendering
{
	RenderTarget::RenderTarget() :
		RenderTarget(Ref<Image>(), Vector4::Zero)
	{}

	RenderTarget::RenderTarget(Ref<Image> mainImage, const Vector4& clearValue) :
		MainImage(mainImage),
		ResolveImage(),
		ClearValue(clearValue)
	{}

	void RenderTarget::SetColorClearValue(const Color& clearColor)
	{
		Color gammaColor = clearColor.AsLinear();
		ClearValue = Vector4(gammaColor.R, gammaColor.G, gammaColor.B, gammaColor.A);
	}

	void RenderTarget::SetDepthClearValue(double depthClearValue)
	{
		ClearValue.X = depthClearValue;
	}

	void RenderTarget::SetDepthStencilClearValue(double depthClearValue, uint8 stencilClearValue)
	{
		ClearValue.X = depthClearValue;
		ClearValue.Y = stencilClearValue;
	}

	MeshData::MeshData(
		uint64 id,
		uint64 version,
		const Ref<Buffer>& vertexBuffer,
		uint64 vertexCount,
		const Ref<Buffer>& indexBuffer,
		const BoundingBox& bounds) :
		ID(id),
		Version(version),
		VertexBuffer(vertexBuffer),
		VertexCount(vertexCount),
		IndexBuffer(indexBuffer),
		Bounds(bounds)
	{}

	RenderPassShaderData::RenderPassShaderData(uint64 id, uint64 version, const RenderPassShader& shaderData) :
		ID(id),
		Version(version),
		ShaderData(shaderData)
	{}

	ShaderData::ShaderData(uint64 id, uint64 version, const string& groupTag, const std::unordered_map<string, uint64>& passShaders) :
		ID(id),
		Version(version),
		GroupTag(groupTag),
		RenderPassShaders(passShaders)
	{}

	MaterialData::MaterialData(uint64 id, uint64 shaderID, const ShaderUniformData& uniformData) :
		ID(id),
		ShaderID(shaderID),
		UniformData(uniformData)
	{}

	ObjectData::ObjectData(
		uint64 id,
		const Matrix4x4& modelMatrix,
		uint64 meshID,
		uint64 indexOffset,
		uint64 indexCount,
		uint64 materialID,
		const RectInt& scissorRect,
		const BoundingBox& bounds,
		SharedRef<ExtraObjectData> extraData) :
		ID(id),
		ModelMatrix(modelMatrix),
		MeshID(meshID),
		IndexOffset(indexOffset),
		IndexCount(indexCount),
		MaterialID(materialID),
		ScissorRect(scissorRect),
		Bounds(bounds),
		ExtraData(extraData)
	{}
}