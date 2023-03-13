#pragma once

namespace Coco::Rendering
{
	enum class FilterMode
	{
		Nearest,
		Linear
	};

	enum class RepeatMode
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};
}