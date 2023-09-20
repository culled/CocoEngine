#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Core/Types/Transform.h>

using namespace Coco;
using namespace Coco::Rendering;

class BasicSceneDataProvider :
    public SceneDataProvider
{
private:
    Transform3D _transform;

    ManagedRef<Mesh> _mesh;
    ManagedRef<Shader> _shader;
    ManagedRef<Texture> _texture;
    ManagedRef<Material> _material;

public:
    BasicSceneDataProvider();

    void GatherSceneData(RenderView& renderView) final;
};

