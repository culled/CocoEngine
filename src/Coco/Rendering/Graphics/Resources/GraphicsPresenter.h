#pragma once

#include "GraphicsResource.h"

#include <Coco/Core/Types/Size.h>
#include "GraphicsPresenterTypes.h"
#include "PresenterSurfaceInitializationInfo.h"

namespace Coco::Rendering
{
	class RenderContext;

	/// <summary>
	/// A presenter that can be used with a window for displaying rendered images
	/// </summary>
	class COCOAPI GraphicsPresenter : public IGraphicsResource
	{
	protected:
		GraphicsPresenter() = default;

	public:
		virtual ~GraphicsPresenter() = default;

		/// <summary>
		/// Initializes a surface for a window
		/// </summary>
		/// <param name="surfaceInitInfo">The platform-specific surface initialization info</param>
		virtual void InitializeSurface(PresenterSurfaceInitializationInfo* surfaceInitInfo) = 0;

		/// <summary>
		/// Gets if the window's surface has been intialized
		/// </summary>
		/// <returns>True if the window's surface has been initialized</returns>
		virtual bool IsSurfaceInitialized() const noexcept = 0;

		/// <summary>
		/// Sets the size of the backbuffer. Should be called when a window resize occurs
		/// </summary>
		/// <param name="backbufferSize">The new size of the window backbuffer</param>
		virtual void SetBackbufferSize(const SizeInt& backbufferSize) = 0;

		/// <summary>
		/// Gets the size of the backbuffer
		/// </summary>
		/// <returns></returns>
		virtual SizeInt GetBackbufferSize() const noexcept = 0;

		/// <summary>
		/// Sets the desired vertical sync mode of this presenter.
		/// There is no guarantee that the mode is supported, and a supported mode will be used as a fallback
		/// </summary>
		/// <param name="mode">The desired vertical sync mode</param>
		virtual void SetVSyncMode(VerticalSyncMode mode) = 0;

		/// <summary>
		/// Gets the vertical sync mode this presenter is using
		/// </summary>
		/// <returns>The vertical sync mode that this presenter is using</returns>
		virtual VerticalSyncMode GetVSyncMode() const = 0;

		/// <summary>
		/// Gets a render context that can be used for rendering
		/// </summary>
		/// <param name="renderContext">The pointer to the acquired render context</param>
		/// <returns>True if a render context was acquired</returns>
		virtual bool GetRenderContext(RenderContext*& renderContext) = 0;

		/// <summary>
		/// Queues the given render context for presentation
		/// </summary>
		/// <param name="renderContext">The render context</param>
		/// <returns>True if the frame was queued for presenting</returns>
		virtual bool Present(RenderContext* renderContext) = 0;
	};
}