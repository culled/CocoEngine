//
// Created by cullen on 3/4/26.
//

#include "RenderService.h"
#include "RenderGraph/RenderGraph.h"

#include "Coco/Core/Engine.h"
#include "Coco/Core/Types/Sorting/QSorter.h"
#include "Texture.h"

namespace Coco
{
    FinalRenderTarget::FinalRenderTarget(uint64 id, Ref<GraphicsSurface> targetSurface) :
        ID(id),
        TargetSurface(targetSurface)
    {}

    RenderService::RenderService(Engine* engine) :
        EngineService(engine),
        _renderingPlatform(dynamic_cast<RenderingEnginePlatform*>(engine->GetPlatform())),
        _graphicsPlatform(nullptr),
        _renderTickListener(this, &RenderService::RenderTick, RenderTickOrder),
        _renderListenersNeedSorting(false),
        _lastFrameStats()
    {
        if (!_renderingPlatform)
            throw Exception("Platform does not support rendering");

        _renderTickListener.ListenTo(*engine->GetMainLoop());
        //_renderer2D = CreateDefaultUnique<Renderer2D>();

        COCO_ENGINE_LOG_VERBOSE("Created RenderService");
    }

    RenderService::~RenderService()
    {
        //_renderer2D.reset();
        _defaultCheckerTexture.reset();

        //while (!_renderListeners.IsEmpty())
        //{
        //    _renderListeners.Front()->StopListening();
        //}

        _finalRenderTargets.Clear();
        _renderTickListener.StopListening();
        _graphicsPlatform.reset();

        COCO_ENGINE_LOG_VERBOSE("Destroyed RenderService");
    }

    void RenderService::AddRenderListener(RenderListener& listener)
    {
        if (listener.IsListening())
            return;

        _renderListeners.Append(&listener);
        _renderListenersNeedSorting = true;
    }

    void RenderService::RemoveRenderListener(const RenderListener& listener)
    {
        _renderListeners.RemoveIf([listener = &listener](const RenderListener* otherListener)
        {
            return listener == otherListener;
        });
    }

    void RenderService::AddFinalRenderTarget(uint64 targetID, Ref<GraphicsSurface> surface)
    {
        if (_finalRenderTargets.Contains(targetID))
            return;

        _finalRenderTargets.Emplace(targetID, targetID, surface);
    }

    void RenderService::RemoveFinalRenderTarget(uint64 targetID)
    {
        _finalRenderTargets.Remove(targetID);
    }

    void RenderService::CreateDefaultResources()
    {
        CreateDefaultCheckerTexture();
    }

    void RenderService::CreateDefaultCheckerTexture()
    {
        constexpr uint32 size = 8;

        _defaultCheckerTexture = _engine->GetResourceManager()->CreateResource<Texture>(
            "DefaultCheckerTexture",
            ImageDescription::Create2D(
                size, size,
                ImagePixelFormat::RGBA8,
                ImageColorSpace::sRGB,
                ImageUsageFlags::Sampled,
                false
            ),
            ImageSamplerDescription::NearestRepeat
        );

        Array<uint8> pixelData(size * size * 4, 0);

        for (uint64 x = 0; x < size; x++)
        {
            for (uint64 y = 0; y < size; y++)
            {
                const uint64 baseIndex = ((x * size) + y) * 4;
                pixelData[baseIndex + 0] = (x % 2) ? 255 : 0;
                pixelData[baseIndex + 1] = (y % 2) ? 255 : 0;
                pixelData[baseIndex + 2] = ((x + y) % 2) ? 255 : 0;
                pixelData[baseIndex + 3] = 255;
            }
        }

        _defaultCheckerTexture->SetPixels(pixelData.Data(), pixelData.GetCount());

        COCO_ENGINE_LOG_VERBOSE("Created default checker texture");
    }

    void RenderService::RenderTick(const TickInfo& tickInfo)
    {
        if (!_graphicsPlatform)
            return;

        if (_renderListenersNeedSorting)
            SortRenderListeners();

        Ref<RenderFrame> frame = _graphicsPlatform->GetCurrentRenderFrame();

        for (auto& pair : _finalRenderTargets)
        {
            if (pair.second.TargetSurface)
                RenderForSurface(pair.first, pair.second.TargetSurface, frame, tickInfo);
        }

        _lastFrameStats = frame->GetStats();
        _graphicsPlatform->NextFrame();

        //if (_renderer2D)
        //    _renderer2D->Reset();
    }

    void RenderService::SortRenderListeners()
    {
        QSorter<RenderListener*> sorter([](const RenderListener* a, const RenderListener* b)
        {
            return a->GetOrder() < b->GetOrder();
        });

        sorter.Sort(_renderListeners);
        _renderListenersNeedSorting = false;
    }

    void RenderService::RenderForSurface(uint64 targetID, Ref<GraphicsSurface> surface, Ref<RenderFrame> renderFrame, const TickInfo& tickInfo)
    {
        surface->RebuildIfNeeded();

        Sizei surfaceSize = surface->GetFramebufferSize();

        RenderScene scene = renderFrame->CreateRenderScene(surfaceSize);

        RenderGraph graph(_graphicsPlatform.get(), renderFrame->GetFrameAllocator(), surfaceSize);
        graph.AddColorAttachment(ImageColorSpace::sRGB);

        for (const auto& listener : _renderListeners)
        {
            COCO_ASSERT(listener->IsListening(), "Listener wasn't connected");

            listener->Dispatch(targetID, graph, scene);
        }

        if (graph.Compile())
        {
            renderFrame->Render(std::move(graph), std::move(scene), surface);
        }
        else
        {
            COCO_ENGINE_LOG_ERROR("RenderGraph compilation failed");
        }
    }
} // Coco