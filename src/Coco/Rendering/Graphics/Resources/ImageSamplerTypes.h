#pragma once

namespace Coco::Rendering
{
	/// @brief Image sampler filtering modes
	enum class FilterMode
	{
		Nearest,
		Linear
	};

	/// @brief Image sampler repeat modes
	enum class RepeatMode
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};
}