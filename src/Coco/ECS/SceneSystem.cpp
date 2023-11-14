#include "ECSpch.h"
#include "SceneSystem.h"

namespace Coco::ECS
{
    SceneSystem::SceneSystem(SharedRef<Scene> scene) :
        _scene(scene)
    {}
}