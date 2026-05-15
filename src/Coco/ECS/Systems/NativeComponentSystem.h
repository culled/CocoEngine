//
// Created by cullen on 5/10/26.
//

#ifndef COCOENGINE_NATIVECOMPONENTSYSTEM_H
#define COCOENGINE_NATIVECOMPONENTSYSTEM_H
#include "Coco/ECS/Scene.h"

namespace Coco
{
    class NativeComponentSystem
    {
    public:
        /// @brief The order that this system's tick callback will be called
        static constexpr int TickOrder = 0;

        /// @brief Updates the NativeComponents in the given scene
        /// @param scene The scene being ticked
        /// @param tickInfo The tick info
        static void Tick(Scene& scene, const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_NATIVECOMPONENTSYSTEM_H