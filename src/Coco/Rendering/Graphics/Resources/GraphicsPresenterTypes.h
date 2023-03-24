#pragma once

namespace Coco::Rendering
{
	/// @brief Vertical sync modes
	enum class VerticalSyncMode
	{
		/// @brief No vertical sync is used and rendered images will be displayed immediately upon completion. This will cause screen tearing
		Disabled,

		/// @brief Basic vertical sync. The engine's tick frequency is locked to the monitor refresh rate. This prevents screen tearing
		Enabled,

		/// @brief Triple buffer vertical sync. A pair of backbuffers can be rendered to as fast as possible and the most recent one is then presented. This prevents screen tearing
		Mailbox
	};
}