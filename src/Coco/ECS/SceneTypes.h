#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Math/Math.h>

namespace Coco::ECS
{
	using SceneID = uint64_t;

	constexpr SceneID RootSceneID = 0;
	constexpr SceneID InvalidSceneID = Math::MaxValue<SceneID>();
}