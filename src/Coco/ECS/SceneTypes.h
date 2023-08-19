#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Math/Math.h>

namespace Coco::ECS
{
	/// @brief The ID of a scene
	using SceneID = uint64_t;

	/// @brief The ID of the root scene
	constexpr SceneID RootSceneID = 0;
}