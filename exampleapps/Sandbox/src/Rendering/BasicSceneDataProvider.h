#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>

using namespace Coco;
using namespace Coco::Rendering;

class BasicSceneDataProvider :
    public SceneDataProvider
{
private:
    Vector3 _position;
    Quaternion _rotation;
    Vector3 _scale;

    UniqueRef<Mesh> _mesh;
    UniqueRef<Shader> _shader;
public:
    BasicSceneDataProvider();

    void GatherSceneData(RenderView& renderView) final;
};

