#pragma once

namespace Coco::Rendering
{
	/// @brief Types of v-sync modes
	enum class VSyncMode
	{
		/// @brief The new image is presented immediately (may result in screen-tearing)
		Immediate,

		/// @brief Presentation is halted until the next vertical blank (may increase latency)
		EveryVBlank
	};

	/// @brief A surface that a GraphicsPresenter can present images to
	struct GraphicsPresenterSurface
	{
		virtual ~GraphicsPresenterSurface() = default;
	};
}