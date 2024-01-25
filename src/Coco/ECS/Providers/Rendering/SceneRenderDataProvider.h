#pragma once
#include <Coco/Rendering/Providers/SceneDataProvider.h>
#include "../../Scene.h"

using namespace Coco::Rendering;

namespace Coco::ECS
{
    /// @brief Provides entities with MeshRendererComponents and Transform3DComponents to a RenderView for rendering
    class SceneRender3DDataProvider :
        public SceneDataProvider
    {
    private:
        SharedRef<Scene> _scene;

    public:
        SceneRender3DDataProvider(SharedRef<Scene> scene);

        // Inherited via SceneDataProvider
        void GatherSceneData(RenderView& renderView) override;
    };
}