#pragma once
#include <Coco/Rendering/Shader.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/ECS/Scene.h>

using namespace Coco;
using namespace Coco::Rendering;

class SceneDataProvider3D
{
public:
    static void SetupScene(SharedRef<ECS::Scene> scene);
};

