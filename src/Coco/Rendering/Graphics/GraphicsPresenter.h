#pragma once

#include <Coco/Core/Types/Size.h>
#include <Coco/Core/Types/Refs.h>
#include "GraphicsPresenterTypes.h"

namespace Coco::Rendering
{
	class Image;
	class RenderContext;
	class GraphicsSemaphore;

	/// @brief Allows images to be presented onto GUI windows
	class GraphicsPresenter
	{
	public:
		virtual ~GraphicsPresenter() = default;

		/// @brief Gets the ID of this presenter
		/// @return This presenter's ID
		virtual uint64 GetID() const = 0;

		/// @brief Initializes the surface that this presenter will use for presentation
		/// @param surface The surface
		virtual void InitializeSurface(const GraphicsPresenterSurface& surface) = 0;

		/// @brief Determines if this presenter's surface is already initialized
		/// @return True if this presenter has a valid surface
		virtual bool SurfaceInitialized() const = 0;

		/// @brief Prepares this presenter for rendering
		/// @param outContext The RenderContext to use for rendering
		/// @param outBackbuffer The image that can be used for rendering to
		/// @return True if preparations were successful
		virtual bool PrepareForRender(Ref<RenderContext>& outContext, Ref<Image>& outBackbuffer) = 0;

		/// @brief Queues the previously prepared RenderContext for presentation
		/// @param frameCompletedSemaphore The semaphore to wait on before presenting the image
		/// @return True if the presentation was queued successfully
		virtual bool Present(Ref<GraphicsSemaphore> frameCompletedSemaphore) = 0;

		/// @brief Sets this presenter's v-sync mode
		/// @param mode The v-sync mode
		virtual void SetVSync(VSyncMode mode) = 0;

		/// @brief Gets this presenter's v-sync mode
		/// @return The v-sync mode
		virtual VSyncMode GetVSync() const = 0;

		/// @brief Sets this presenter's framebuffer size
		/// @param size The size of the framebuffer
		virtual void SetFramebufferSize(const SizeInt& size) = 0;

		/// @brief Gets this presenter's framebuffer size
		/// @return The framebuffer size
		virtual SizeInt GetFramebufferSize() const = 0;

		/// @brief Sets the maximum number of frames that can be queued to present at once
		/// @param maxFramesInFlight The maximum number of queued frames
		virtual void SetMaximumFramesInFlight(uint8 maxFramesInFlight) = 0;

		/// @brief Gets the maximum number of frames that can be queued to present at once
		/// @return The maximum frames in flight
		virtual uint8 GetMaximumFramesInFlight() const = 0;
	};
}