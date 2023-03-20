#pragma once

namespace Coco::Rendering
{
	/// <summary>
	/// Image sampler filtering modes
	/// </summary>
	enum class FilterMode
	{
		Nearest,
		Linear
	};

	/// <summary>
	/// Image sampler repeat modes
	/// </summary>
	enum class RepeatMode
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};
}