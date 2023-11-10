#pragma once

namespace Coco::Rendering
{
	/// @brief Clear modes for an attachment
	enum class AttachmentClearMode
	{
		/// @brief The attachment should not be cleared
		DontClear,

		/// @brief The attachment should be cleared
		Clear
	};
}