//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_SANDBOXAPPLICATION_H
#define COCOENGINE_SANDBOXAPPLICATION_H
#include <Coco/Core/Application.h>
#include <Coco/Core/ProcessLoop/TickListener.h>

#include "Coco/Core/Memory/Refs.h"
#include <Coco/Windowing/Window.h>

#include "Coco/ECS/Scene.h"
#include "Coco/Rendering/Material.h"
#include "Coco/Rendering/Mesh.h"
#include "Coco/Rendering/RenderListener.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Rendering/Texture.h"
#include "Coco/Rendering/2D/Tilemap/TileMap.h"

namespace Coco
{
    class RenderScene;
    class RenderGraph;
}

using namespace Coco;

class SandboxApplication :
    public Application
{
public:
    SandboxApplication(Engine* engine);
    ~SandboxApplication();

    void Start() override;
    Version GetVersion() const override { return Version(0, 1, 0); }

private:
    TickListener _tickListener;
    RenderListener _renderListener;
    Ref<Window> _window;
    SharedPtr<Shader> _shader;
    SharedPtr<Texture> _spriteTexture;
    SharedPtr<Texture> _texture2;
    SharedPtr<Scene> _scene;
    SharedPtr<TileMap> _tileMap;
    Entity _cameraEntity;
    Entity _tilemapEntity;
    Entity _spriteEntity;
    Entity _spriteEntity2;

private:
    void CreateServices();
    void CreateResources();
    void CreateScene();
    void OnTick(const TickInfo& tickInfo);
    void RenderSceneCallback(uint64 targetID, RenderGraph& graph, RenderScene& scene);
    void DrawTilemap(RenderGraphResourceRef colorRef, RenderGraph& graph, RenderScene& scene);
    void DrawSprites(RenderGraphResourceRef colorRef, RenderGraph& graph, RenderScene& scene);
};


#endif //COCOENGINE_SANDBOXAPPLICATION_H