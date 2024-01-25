#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Size.h>
#include "GraphicsResource.h"
#include "PresenterTypes.h"

namespace Coco::Rendering
{
	/// @brief Holds backbuffers for a window and allows them to be written to and displayed
	class Presenter :
		public GraphicsResource
	{
	public:
		virtual ~Presenter() = default;

		/// @brief Determines if this presenter has a window surface
		/// @return True if this presenter has a window surface
		virtual bool HasSurface() const = 0;

		/// @brief Sets the window surface. NOTE: this only needs to be called if HasSurface() returns false
		/// @param surface The window surface
		virtual void SetSurface(UniqueRef<PresenterSurface>&& surface) = 0;

		/// @brief Sets the size of the framebuffer
		/// @param size The framebuffer size
		virtual void SetFramebufferSize(const SizeInt& size) = 0;

		/// @brief Gets the current size of the framebuffer
		/// @return The framebuffer size
		virtual const SizeInt& GetFramebufferSize() = 0;

		/// @brief Sets the vertical synchronization mode
		/// @param mode The VSync mode
		virtual void SetVSyncMode(VSyncMode mode) = 0;

		/// @brief Gets the current vertical synchronization mode
		/// @return The VSync mode
		virtual VSyncMode GetVSyncMode() const = 0;

	protected:
		Presenter(const GraphicsResourceID& id);
	};
}