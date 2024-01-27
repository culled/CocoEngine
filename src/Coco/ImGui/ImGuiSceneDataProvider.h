#pragma once
#include <Coco\Rendering\Providers\SceneDataProvider.h>
#include <Coco/Core/Resources/ResourceTypes.h>

struct ImGuiViewport;

namespace Coco::Rendering
{
    class Mesh;
    class Texture;
}

namespace Coco::ImGuiCoco
{
    struct ImGuiExtraData
    {
        RectInt ScissorRect;
        ResourceID OverrideTextureID;

        ImGuiExtraData(const RectInt& scissorRect, const ResourceID& overrideTextureID);
    };

    class ImGuiSceneDataProvider :
        public Rendering::SceneDataProvider
    {
    public:
        ImGuiSceneDataProvider(ImGuiViewport* viewport);

        static void DestroyViewportMesh(ImGuiViewport* viewport);

        // Inherited via SceneDataProvider
        void GatherSceneData(Rendering::RenderView& renderView) override;

    private:
        ImGuiViewport* _viewport;

    private:
        static ResourceID CreateResourceIDForViewport(ImGuiViewport* viewport);

        /// @brief Gets or creates a mesh for a viewport
        /// @param viewport The viewport
        /// @return The mesh for the viewport
        static SharedRef<Rendering::Mesh> GetOrCreateViewportMesh(ImGuiViewport* viewport);
    };
}
