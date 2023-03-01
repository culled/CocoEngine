#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Size.h>

#include "GraphicsPlatformTypes.h"
#include "PresenterSurfaceInitializationInfo.h"
#include "GraphicsFence.h"
#include "GraphicsSemaphore.h"
#include "RenderView.h"
#include "RenderContext.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::Rendering
{
	/// <summary>
	/// A result from a GraphicsPresenter operation
	/// </summary>
	enum class GraphicsPresenterResult
	{
		Success,
		NeedsReinitialization,
		Failure
	};

	/// <summary>
	/// A presenter that can be used with a window for displaying rendered images
	/// </summary>
	class COCOAPI GraphicsPresenter
	{
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
		virtual bool IsSurfaceInitialized() const = 0;

		/// <summary>
		/// Sets the size of the backbuffer. Should be called when a window resize occurs
		/// </summary>
		/// <param name="backbufferSize">The new size of the window backbuffer</param>
		virtual void SetBackbufferSize(const SizeInt& backbufferSize) = 0;

		/// <summary>
		/// Gets the size of the backbuffer
		/// </summary>
		/// <returns></returns>
		virtual SizeInt GetBackbufferSize() const = 0;

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

		virtual Managed<RenderContext> CreateRenderContext(const Ref<RenderView>& view, const Ref<RenderPipeline>& pipeline, int backbufferImageIndex) = 0;

		/// <summary>
		/// Acquires the index of the next backbuffer to draw to
		/// </summary>
		/// <param name="timeoutNs">The timeout to wait for, in nanoseconds</param>
		/// <param name="imageAvailableFence">A fence to wait on for the image to become available</param>
		/// <param name="imageAvailableSemaphore">A semaphore to wait on for the image to become available</param>
		/// <param name="backbufferImageIndex">The index of the backbuffer to use</param>
		/// <returns>The operation result</returns>
		virtual GraphicsPresenterResult AcquireNextBackbuffer(
			unsigned long long timeoutNs, 
			int& backbufferImageIndex) = 0;

		/// <summary>
		/// Queues the backbuffer at the given index for presentation
		/// </summary>
		/// <param name="backbufferImageIndex">The index of the backbuffer</param>
		/// <param name="renderCompleteSemaphore">A semaphore to wait on for the image to finish rendering</param>
		/// <returns>The operation result</returns>
		virtual GraphicsPresenterResult Present(int backbufferImageIndex) = 0;
	};
}