#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Rendering/Mesh.h>

#include "RenderView.h"
#include "Shader.h"

namespace Coco::Rendering
{
	class IRenderPass;
	class RenderPipeline;

	/// <summary>
	/// A context that can be used for rendering
	/// </summary>
	class COCOAPI RenderContext
	{
		friend RenderPipeline;

	protected:
		/// <summary>
		/// The render view used for rendering
		/// </summary>
		Ref<RenderView> RenderView;

		/// <summary>
		/// The currently rendering render pass
		/// </summary>
		Ref<IRenderPass> CurrentRenderPass = nullptr;

		/// <summary>
		/// The index in the RenderPipeline of the current render pass
		/// </summary>
		uint CurrentRenderPassIndex = 0;

	protected:
		RenderContext(Ref<Rendering::RenderView> renderView);

	public:
		virtual ~RenderContext();

		/// <summary>
		/// Begins rendering for a scene
		/// </summary>
		/// <returns>True if the context began rendering successfully</returns>
		virtual bool Begin() = 0;

		/// <summary>
		/// Ends rendering for a scene
		/// </summary>
		virtual void End() = 0;

		/// <summary>
		/// Sets the size and offset of the viewport to use
		/// </summary>
		/// <param name="offset">The offset of the viewport</param>
		/// <param name="size">The size of the viewport</param>
		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) = 0;

		/// <summary>
		/// Sets the given shader as the one that will be used to draw subsequent geometry
		/// </summary>
		/// <param name="shader">The shader to use</param>
		virtual void UseShader(Ref<Shader> shader) = 0;

		/// <summary>
		/// Draws a number of triangles
		/// </summary>
		/// <param name="indexCount">The number of triangle indices to draw</param>
		/// <param name="indexOffset">The offset of the index to start drawing</param>
		/// <param name="vertexOffset">The offset of the vertex to start drawing</param>
		/// <param name="instanceCount">The number of instances to draw</param>
		/// <param name="instanceOffset">The offset of the instance to start drawing</param>
		virtual void DrawIndexed(uint indexCount, uint indexOffset, uint vertexOffset, uint instanceCount, uint instanceOffset) = 0;

		/// <summary>
		/// Draws a mesh
		/// </summary>
		/// <param name="mesh">The mesh to draw</param>
		virtual void Draw(const Ref<Mesh>& mesh) = 0;

		/// <summary>
		/// Restores the viewport to the size and offset specified by the RenderView
		/// </summary>
		void RestoreViewport();

	private:
		/// <summary>
		/// Sets the current render pass
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="passIndex">The index of the pass in the current render pipeline</param>
		void SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex);
	};
}