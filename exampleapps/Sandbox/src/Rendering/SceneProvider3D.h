#pragma once
#include <Coco\Rendering\Providers\SceneDataProvider.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Texture.h>

using namespace Coco;

class SceneProvider3D :
    public Rendering::SceneDataProvider
{
public:
    SceneProvider3D();

    // Inherited via SceneDataProvider
    void GatherSceneData(Rendering::RenderView& renderView) override;

private:
    SharedRef<Rendering::Mesh> _mesh;
    SharedRef<Rendering::Material> _material;
    SharedRef<Rendering::Texture> _texture;

private:
    void CreateResources();
};

