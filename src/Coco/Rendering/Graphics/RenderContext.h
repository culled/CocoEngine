#pragma once

#include "GraphicsResource.h"
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Rect.h>
#include "ShaderUniformTypes.h"

namespace Coco::Rendering
{
	class Shader;
	class Mesh;
	class Material;
	struct Submesh;

	class RenderContext :
		public GraphicsResource
	{
	public:
		virtual ~RenderContext() = default;

		virtual void SetViewportRect(const RectInt& viewportRect) = 0;
		virtual void SetScissorRect(const RectInt& scissorRect) = 0;
		virtual void SetShader(const SharedRef<Shader>& shader) = 0;
		virtual void Draw(const SharedRef<Mesh>& mesh, const Submesh& submesh) = 0;
		virtual void DrawIndexed(const SharedRef<Mesh>& mesh, uint64 indexOffset, uint64 indexCount) = 0;

		virtual void SetGlobalUniforms(std::span<const ShaderUniformValue> uniforms) = 0;
		virtual void SetGlobalShaderUniforms(std::span<const ShaderUniformValue> uniforms) = 0;
		virtual void SetMaterial(const SharedRef<Material>& material) = 0;
		virtual void SetInstanceUniforms(uint64 instanceID, std::span<const ShaderUniformValue> uniforms) = 0;
		virtual void SetDrawUniforms(std::span<const ShaderUniformValue> uniforms) = 0;

	protected:
		RenderContext(const GraphicsResourceID& id);
	};
}