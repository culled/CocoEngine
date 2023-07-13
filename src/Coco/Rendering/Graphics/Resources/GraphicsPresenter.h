#pragma once

#include <Coco/Rendering/RenderingResource.h>

#include <Coco/Core/Types/Size.h>
#include "GraphicsPresenterTypes.h"
#include "PresenterSurfaceInitializationInfo.h"

namespace Coco::Rendering
{
	class RenderContext;
	class Image;

	/// @brief A presenter that can be used with a window for displaying rendered images
	class COCOAPI GraphicsPresenter : public RenderingResource
	{
	public:
		GraphicsPresenter(const ResourceID& id, const string& name) : RenderingResource(id, name) {}
		virtual ~GraphicsPresenter() = default;

		/// @brief Initializes a surface for a window
		/// @param surfaceInitInfo The platform-specific surface initialization info
		virtual void InitializeSurface(const PresenterSurfaceInitializationInfo& surfaceInitInfo) = 0;

		/// @brief Gets if the window's surface has been intialized
		/// @return True if the window's surface has been initialized
		virtual bool IsSurfaceInitialized() const noexcept = 0;

		/// @brief Sets the size of the backbuffer. Should be called when a window resize occurs
		/// @param backbufferSize The new size of the window backbuffer
		virtual void SetBackbufferSize(const SizeInt& backbufferSize) = 0;

		/// @brief Gets the size of the backbuffer
		/// @return The size of the backbuffer
		virtual SizeInt GetBackbufferSize() const noexcept = 0;

		/// @brief Sets the desired vertical sync mode of this presenter.
		/// There is no guarantee that the mode is supported, and a supported mode will be used as a fallback
		/// @param mode The desired vertical sync mode
		virtual void SetVSyncMode(VerticalSyncMode mode) = 0;

		/// @brief Gets the vertical sync mode this presenter is using
		/// @return The vertical sync mode that this presenter is using
		virtual VerticalSyncMode GetVSyncMode() const = 0;

		/// @brief Gets a render context that can be used for rendering
		/// @param renderContext Will be set to an acquired render context
		/// @param backbuffer Will be set to the backbuffer image to use for rendering
		/// @return True if a render context was acquired
		virtual bool PrepareForRender(Ref<RenderContext>& renderContext, Ref<Image>& backbuffer) = 0;

		/// @brief Queues the given render context for presentation
		/// @param renderContext The render context
		/// @return True if the frame was queued for presenting
		virtual bool Present(Ref<RenderContext> renderContext) = 0;
	};
}