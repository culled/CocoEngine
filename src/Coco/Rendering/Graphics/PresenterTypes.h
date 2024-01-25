#pragma once

namespace Coco::Rendering
{
	/// @brief Types of v-sync modes
	enum class VSyncMode
	{
		/// @brief The new image is presented immediately (may result in screen-tearing)
		Immediate,

		/// @brief Presentation is halted until the next vertical blank (may increase latency)
		EveryVBlank,

		/// @brief Swaps between two backbuffers immediately until the next vertical blank, when the newest backbuffer is swapped with the frontbuffer
		Mailbox
	};

	/// @brief A surface that a Presenter can present images to (usually used by a Window)
	struct PresenterSurface
	{
		virtual ~PresenterSurface() = default;
	};
}