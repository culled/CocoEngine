#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Graphics/Buffer.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>
#include "RenderView.h"

#include "GraphicsResource.h"
#include "RenderContextTypes.h"

namespace Coco::Rendering
{
	class IRenderPass;
	class RenderPipeline;

	/// <summary>
	/// An object that holds global uniform data that can be directly pushed to a shader
	/// </summary>
	struct GlobalUniformObject
	{
		float Projection[4 * 4];	// 64 bytes
		float View[4 * 4];			// 64 bytes
		uint8_t Padding[128];		// 128 bytes - padding

		GlobalUniformObject() noexcept;
		GlobalUniformObject(const RenderView* renderView) noexcept;

	private:
		/// <summary>
		/// Populates a float array pointer with matrix values
		/// </summary>
		/// <param name="destinationMatrixPtr">The pointer to the first element of the float array</param>
		/// <param name="sourceMatrix">The matrix</param>
		void PopulateMatrix(float* destinationMatrixPtr, const Matrix4x4& sourceMatrix) noexcept;
	};

	/// <summary>
	/// A context that can be used for rendering
	/// </summary>
	class COCOAPI RenderContext : public IGraphicsResource
	{
		friend RenderPipeline;

	protected:
		/// <summary>
		/// The render view used for rendering
		/// </summary>
		Ref<RenderView> RenderView;

		/// <summary>
		/// The render pipeline being used for rendering
		/// </summary>
		Ref<RenderPipeline> CurrentPipeline;

		/// <summary>
		/// The currently rendering render pass
		/// </summary>
		Ref<IRenderPass> CurrentRenderPass = nullptr;

		/// <summary>
		/// The index in the RenderPipeline of the current render pass
		/// </summary>
		uint CurrentRenderPassIndex = 0;

		/// <summary>
		/// The global uniform object
		/// </summary>
		GlobalUniformObject GlobalUO;

	protected:
		RenderContext() = default;

	public:
		virtual ~RenderContext() = default;

		RenderContext(const RenderContext&) = delete;
		RenderContext(RenderContext&&) = delete;

		RenderContext& operator=(const RenderContext&) = delete;
		RenderContext& operator=(RenderContext&&) = delete;

		/// <summary>
		/// Begins rendering for a scene
		/// </summary>
		/// <returns>True if the context began rendering successfully</returns>
		bool Begin(Ref<Rendering::RenderView> renderView, Ref<RenderPipeline> pipeline);

		/// <summary>
		/// Ends rendering for a scene
		/// </summary>
		void End();

		/// <summary>
		/// Resets this context to begin rendering a new scene
		/// </summary>
		void Reset();

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
		/// Sets the given material as the one that will be used to draw subsequent geometry
		/// </summary>
		/// <param name="material">The material to use</param>
		virtual void UseMaterial(Ref<Material> material) = 0;

		/// <summary>
		/// Draws a mesh
		/// </summary>
		/// <param name="mesh">The mesh to draw</param>
		/// <param name="modelMatrix">The model matrix</param>
		virtual void Draw(const Mesh* mesh, const Matrix4x4& modelMatrix) = 0;

		/// <summary>
		/// Restores the viewport to the size and offset specified by the RenderView
		/// </summary>
		void RestoreViewport();

		/// <summary>
		/// Gets if this render context can be immediately used for rendering
		/// </summary>
		/// <returns>True if this render context can immediately be used for rendering</returns>
		virtual bool IsAvaliableForRendering() noexcept = 0;

		/// <summary>
		/// Waits for this render context's rendering to complete
		/// </summary>
		virtual void WaitForRenderingCompleted() = 0;

	protected:
		/// <summary>
		/// Called when this render context is starting to render a scene
		/// </summary>
		/// <returns>True if rendering started successfully</returns>
		virtual bool BeginImpl() = 0;

		/// <summary>
		/// Ends rendering the current scene 
		/// </summary>
		virtual void EndImpl() = 0;

		/// <summary>
		/// Resets this context
		/// </summary>
		virtual void ResetImpl() = 0;

	private:
		/// <summary>
		/// Sets the current render pass
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="passIndex">The index of the pass in the current render pipeline</param>
		void SetCurrentRenderPass(Ref<IRenderPass> renderPass, uint passIndex) noexcept;
	};
}