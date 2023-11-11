#pragma once

namespace Coco::Rendering
{
	/// @brief Options for an attachment
	enum class AttachmentOptionFlags
	{
		None = 0,

		/// @brief The attachment should be cleared
		Clear = 1 << 0,

		/// @brief The attachment's data should be preserved between renders
		Preserve = 1 << 1,
	};

	constexpr AttachmentOptionFlags operator|(const AttachmentOptionFlags& a, const AttachmentOptionFlags& b)
	{ 
		return static_cast<AttachmentOptionFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	constexpr void operator|=(AttachmentOptionFlags& a, const AttachmentOptionFlags& b)
	{
		a = a | b;
	}

	constexpr AttachmentOptionFlags operator&(const AttachmentOptionFlags& a, const AttachmentOptionFlags& b)
	{
		return static_cast<AttachmentOptionFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	constexpr void operator&=(AttachmentOptionFlags& a, const AttachmentOptionFlags& b)
	{
		a = a & b;
	}
}