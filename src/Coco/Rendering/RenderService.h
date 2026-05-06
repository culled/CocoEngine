//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_RENDERSERVICE_H
#define COCOENGINE_RENDERSERVICE_H
#include <Coco/Core/EngineService.h>

#include "RenderingEnginePlatform.h"
#include "RenderListener.h"
#include "RenderScene.h"

//#include "2D/Renderer2D.h"

#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/ProcessLoop/TickListener.h"
#include "Graphics/GraphicsPlatform.h"

namespace Coco
{
    class RenderGraph;
    class Texture;

    struct FinalRenderTarget
    {
        uint64 ID;
        Ref<GraphicsSurface> TargetSurface;

        FinalRenderTarget(uint64 id, Ref<GraphicsSurface> targetSurface);
    };

    class RenderService : public EngineService
    {
    public:
        static constexpr int RenderTickOrder = 9000;

    public:
        RenderService(Engine* engine);
        ~RenderService();

        RenderingEnginePlatform* GetRenderingPlatform() { return _renderingPlatform; }

        template<typename PlatformType, typename ... Args>
        GraphicsPlatform* CreatePlatform(Args&& ... args)
        {
            _graphicsPlatform = CreateDefaultUnique<PlatformType>(this, std::forward<Args>(args)...);

            CreateDefaultResources();
            return _graphicsPlatform.get();
        }

        GraphicsPlatform* GetGraphicsPlatform() { return _graphicsPlatform.get(); }
        const GraphicsPlatform* GetGraphicsPlatform() const { return _graphicsPlatform.get(); }
        //Renderer2D* GetRenderer2D() { return _renderer2D.get(); }

        void AddRenderListener(RenderListener& listener);
        void RemoveRenderListener(const RenderListener& listener);

        void AddFinalRenderTarget(uint64 targetID, Ref<GraphicsSurface> surface);
        void RemoveFinalRenderTarget(uint64 targetID);

        SharedPtr<Texture> GetDefaultCheckerTexture() { return _defaultCheckerTexture; }

        const RenderFrameStats& GetLastFrameStats() const { return _lastFrameStats; }

    private:
        RenderingEnginePlatform* _renderingPlatform;
        UniquePtr<GraphicsPlatform> _graphicsPlatform;
        //UniquePtr<Renderer2D> _renderer2D;
        TickListener _renderTickListener;
        Array<RenderListener*> _renderListeners;
        bool _renderListenersNeedSorting;
        Map<uint64, FinalRenderTarget> _finalRenderTargets;
        SharedPtr<Texture> _defaultCheckerTexture;
        RenderFrameStats _lastFrameStats;

    private:
        void CreateDefaultResources();
        void CreateDefaultCheckerTexture();

        void RenderTick(const TickInfo& tickInfo);
        void SortRenderListeners();
        void RenderForSurface(uint64 targetID, Ref<GraphicsSurface> surface, Ref<RenderFrame> renderFrame, const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_RENDERSERVICE_H