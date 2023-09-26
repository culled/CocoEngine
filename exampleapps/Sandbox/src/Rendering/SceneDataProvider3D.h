#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/MaterialInstance.h>
#include <Coco/Core/Types/Transform.h>

using namespace Coco;
using namespace Coco::Rendering;

class SceneDataProvider3D :
    public SceneDataProvider
{
private:
    Transform3D _transform;

    Ref<Mesh> _mesh;
    Ref<Shader> _shader;
    Ref<Texture> _texture;
    Ref<Material> _material;

public:
    SceneDataProvider3D();

    void GatherSceneData(RenderView& renderView) final;
};

