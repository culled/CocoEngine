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
    class RenderService;

    /// @brief A listener that calls a render callback
    class RenderListener
    {
    public:
        using RenderCallback = std::function<void(uint64, RenderGraph&, RenderScene&)>;

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

        /// @brief Gets the order of this listener. Listeners are called in ascending order
        /// @return The order
        int GetOrder() const { return _order; }

        /// @brief Starts listening for renders
        void Listen();

        void Listen(RenderService& renderService);

        /// @brief Stops listening for renders
        void StopListening();

        /// @brief Gets the listening state of this listener
        /// @return True if this listener is listening for renders
        bool IsListening() const { return _isListening; }

        /// @brief Sets the callback that will be called during rendering
        /// @param callbackFunction The function that will be called during rendering
        void SetCallbackFunction(RenderCallback callbackFunction);

        /// @brief Sets the callback that will be called during rendering
        /// @tparam InstanceType The instance class type
        /// @param instance A pointer to the instance
        /// @param callbackFunction The member function that will be called during rendering
        template<typename InstanceType>
        void SetCallbackFunction(InstanceType* instance, void(InstanceType::* callbackFunction)(uint64, RenderGraph&, RenderScene&))
        {
            COCO_ASSERT(instance, "Instance was null");
            COCO_ASSERT(callbackFunction, "Callback was null");

            _callback = [instance, callbackFunction](uint64 targetID, RenderGraph& graph, RenderScene& scene) { return (instance->*callbackFunction)(targetID, graph, scene); };
        }

        /// @brief Called during rendering to call this listener's callback function
        /// @param targetID The ID of the target being rendered
        /// @param graph The render graph
        /// @param scene The render scene
        void Dispatch(uint64 targetID, RenderGraph& graph, RenderScene& scene);

    private:
        RenderCallback _callback;
        int _order;
        bool _isListening;
    };
} // Coco

#endif //COCOENGINE_RENDERLISTENER_H