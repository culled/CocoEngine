//
// Created by cullen on 5/10/26.
//

#ifndef COCOENGINE_GAMEAPPLICATION_H
#define COCOENGINE_GAMEAPPLICATION_H
#include "WorldManager.h"

#include "Coco/Core/Application.h"
#include "Coco/Rendering/RenderListener.h"

namespace Coco
{
    class RenderScene;
    class RenderGraph;
}

using namespace Coco;

class GameApplication : public Application
{
public:
    GameApplication(Engine* engine);
    ~GameApplication();

    Version GetVersion() const override { return Version(0, 1, 0); }
    void Start() override;
    void Tick(const TickInfo& tickInfo) override;

private:
    UniquePtr<WorldManager> _worldManager;
    RenderListener _renderListener;

    void CreateServices();

    void OnRender(uint64 targetID, RenderGraph& graph, RenderScene& scene);
};


#endif //COCOENGINE_GAMEAPPLICATION_H