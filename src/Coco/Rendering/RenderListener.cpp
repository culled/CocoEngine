//
// Created by cullen on 3/23/26.
//

#include "RenderListener.h"

#include "RenderService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    RenderListener::RenderListener(RenderCallback callback, int order) :
        _callback(std::move(callback)),
        _order(order),
        _isListening(false)
    {}

    RenderListener::~RenderListener()
    {
        if (_isListening)
            StopListening();
    }

    void RenderListener::Listen()
    {
        if (auto engine = Engine::Get())
        {
            if (auto rendering = engine->GetService<RenderService>())
            {
                rendering->AddRenderListener(*this);
                _isListening = true;
            }
        }
    }

    void RenderListener::StopListening()
    {
        if (auto engine = Engine::Get())
        {
            if (auto rendering = engine->GetService<RenderService>())
            {
                rendering->RemoveRenderListener(*this);
                _isListening = false;
            }
        }
    }

    void RenderListener::SetCallbackFunction(RenderCallback callbackFunction)
    {
        _callback = std::move(callbackFunction);
    }

    void RenderListener::Dispatch(uint64 targetID, RenderGraph& graph, RenderScene& scene)
    {
        if (!_callback)
            return;

        _callback(targetID, graph, scene);
    }
} // Coco