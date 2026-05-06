//
// Created by cullen on 3/23/26.
//

#ifndef COCOENGINE_RENDERLISTENER_H
#define COCOENGINE_RENDERLISTENER_H
#include "RenderScene.h"
#include "Coco/Core/ProcessLoop/TickInfo.h"
#include "RenderGraph/RenderGraph.h"

namespace Coco
{
    class RenderListener
    {
    public:
        using RenderCallback = std::function<void(uint64, RenderGraph&, RenderScene&)>;

    public:
        RenderListener(RenderCallback callback, int order);

        template<typename InstanceType>
        RenderListener(InstanceType* instance, void(InstanceType::* callbackFunction)(uint64, RenderGraph&, RenderScene&), int order) :
            RenderListener(nullptr, order)
        {
            SetCallbackFunction(instance, callbackFunction);
        }

        ~RenderListener();

        RenderListener(const RenderListener&) = delete;
        RenderListener& operator= (const RenderListener&) = delete;

        int GetOrder() const { return _order; }
        bool IsListening() const { return _isListening; }

        void Listen();
        void StopListening();

        void SetCallbackFunction(RenderCallback callbackFunction);

        template<typename InstanceType>
        void SetCallbackFunction(InstanceType* instance, void(InstanceType::* callbackFunction)(uint64, RenderGraph&, RenderScene&))
        {
            COCO_ASSERT(instance, "Instance was null");
            COCO_ASSERT(callbackFunction, "Callback was null");

            _callback = [instance, callbackFunction](uint64 targetID, RenderGraph& graph, RenderScene& scene) { return (instance->*callbackFunction)(targetID, graph, scene); };
        }

        void Dispatch(uint64 targetID, RenderGraph& graph, RenderScene& scene);
    private:
        RenderCallback _callback;
        int _order;
        bool _isListening;
    };
} // Coco

#endif //COCOENGINE_RENDERLISTENER_H