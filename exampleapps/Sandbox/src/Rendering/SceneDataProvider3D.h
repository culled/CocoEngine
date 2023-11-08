#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Shader.h>
#include <Coco/Core/Types/Transform.h>

using namespace Coco;
using namespace Coco::Rendering;

class SceneDataProvider3D :
    public SceneDataProvider
{
private:
    Transform3D _boxTransform;
    SharedRef<Mesh> _boxMesh;

    SharedRef<Mesh> _mesh;
    SharedRef<Texture> _texture;
    SharedRef<Texture> _normalTexture;
    SharedRef<Material> _material;

    bool _drawBounds;

public:
    SceneDataProvider3D();

    void SetDrawBounds(bool drawBounds);
    bool GetDrawBounds() const { return _drawBounds; }

    void GatherSceneData(RenderView& renderView) final;
};

