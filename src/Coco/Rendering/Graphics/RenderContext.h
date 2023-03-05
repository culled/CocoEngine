#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Vector.h>

#include "RenderView.h"
#include "Shader.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A context that can be used for rendering
	/// </summary>
	class COCOAPI RenderContext
	{
	protected:
		/// <summary>
		/// The render view used for rendering
		/// </summary>
		Ref<RenderView> RenderView;

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
		/// Restores the viewport to the size and offset specified by the RenderView
		/// </summary>
		void RestoreViewport();

		//virtual void UseShader(const Shader* shader) = 0;
	};
}