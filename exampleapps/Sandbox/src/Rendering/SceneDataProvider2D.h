#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Texture.h>

using namespace Coco;
using namespace Coco::Rendering;

class SceneDataProvider2D :
    public SceneDataProvider
{
private:
    Transform2D _transform;

    Ref<Mesh> _mesh;
    Ref<Shader> _shader;
    Ref<Texture> _texture;
    Ref<Material> _material;

public:
    SceneDataProvider2D();
    ~SceneDataProvider2D();

    void GatherSceneData(RenderView& renderView) final;
};

