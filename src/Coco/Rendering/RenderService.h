//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_RENDERSERVICE_H
#define COCOENGINE_RENDERSERVICE_H
#include <Coco/Core/EngineService.h>

#include "RenderingEnginePlatform.h"
#include "RenderListener.h"
#include "RenderScene.h"

#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/ProcessLoop/TickListener.h"

#include "Gizmos/Gizmos.h"

#include "Graphics/GraphicsPlatform.h"

namespace Coco
{
    class RenderGraph;
    class Texture;

    /// @brief A surface that can be drawn to
    struct FinalRenderTarget
    {
        /// @brief The surface ID
        uint64 ID;

        /// @brief The surface
        Ref<GraphicsSurface> TargetSurface;

        FinalRenderTarget(uint64 id, Ref<GraphicsSurface> targetSurface);
    };

    class RenderService : public EngineService
    {
    public:
        /// @brief The tick order for rendering
        static constexpr int RenderTickOrder = 9000;

        RenderService(Engine* engine);
        ~RenderService();

        /// @brief Gets the engine platform's rendering support
        /// @return The engine platform's rendering support
        RenderingEnginePlatform* GetRenderingPlatform() { return _renderingPlatform; }

        /// @brief Creates the graphics platform
        /// @tparam PlatformType The graphics platform type
        /// @tparam Args The constructor arguments
        /// @param args The arguments to pass to the graphics platform's constructor
        /// @return The graphics platform
        template<typename PlatformType, typename ... Args>
        GraphicsPlatform* CreatePlatform(Args&& ... args)
        {
            _graphicsPlatform = CreateDefaultUnique<PlatformType>(this, std::forward<Args>(args)...);

            CreateDefaultResources();
            return _graphicsPlatform.get();
        }

        /// @brief Gets the graphics platform
        /// @return The graphics platform
        GraphicsPlatform* GetGraphicsPlatform() { return _graphicsPlatform.get(); }

        /// @brief Gets the graphics platform
        /// @return The graphics platform
        const GraphicsPlatform* GetGraphicsPlatform() const { return _graphicsPlatform.get(); }

        /// @brief Adds a listener to be notified when rendering occurs
        /// @param listener The listener
        void AddRenderListener(RenderListener& listener);

        /// @brief Removes a listener, causing it to no longer be notified when rendering occurs
        /// @param listener The listener
        void RemoveRenderListener(const RenderListener& listener);

        /// @brief Adds a final render target, which will be rendered for during the render tick
        /// @param targetID The ID of the target
        /// @param surface The surface of the target
        void AddFinalRenderTarget(uint64 targetID, Ref<GraphicsSurface> surface);

        /// @brief Removes a final render target, and it will no longer be rendered for during the render tick
        /// @param targetID The ID of the target
        void RemoveFinalRenderTarget(uint64 targetID);

        /// @brief Gets the default checker texture (mostly used for errors)
        /// @return The default checker texture
        SharedPtr<Texture> GetDefaultCheckerTexture() { return _defaultCheckerTexture; }

        /// @brief Gets the rendering statistics of the last frame
        /// @return The rendering statistics of the last frame
        const RenderFrameStats& GetLastFrameStats() const { return _lastFrameStats; }

        Gizmos* GetGizmos() { return _gizmos.get(); }

    private:
        RenderingEnginePlatform* _renderingPlatform;
        UniquePtr<GraphicsPlatform> _graphicsPlatform;
        TickListener _renderTickListener;
        Array<RenderListener*> _renderListeners;
        bool _renderListenersNeedSorting;
        Map<uint64, FinalRenderTarget> _finalRenderTargets;
        SharedPtr<Texture> _defaultCheckerTexture;
        RenderFrameStats _lastFrameStats;
        UniquePtr<Gizmos> _gizmos;

        /// @brief Creates the default resources used by the renderer
        void CreateDefaultResources();

        /// @brief Creates the default checker texture resource
        void CreateDefaultCheckerTexture();

        /// @brief The tick handler for the rendering tick
        /// @param tickInfo The tick info
        void RenderTick(const TickInfo& tickInfo);

        /// @brief Sorts the render listeners
        void SortRenderListeners();

        /// @brief Renders for the given surface
        /// @param targetID The surface ID
        /// @param surface The surface
        /// @param renderFrame The current RenderFrame
        /// @param tickInfo The tick info
        void RenderForSurface(uint64 targetID, Ref<GraphicsSurface> surface, Ref<RenderFrame> renderFrame, const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_RENDERSERVICE_H