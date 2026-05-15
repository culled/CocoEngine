//
// Created by cullen on 5/14/26.
//

#ifndef COCOENGINE_SPRITESHEETANIMATIONCOMPONENTSYSTEM_H
#define COCOENGINE_SPRITESHEETANIMATIONCOMPONENTSYSTEM_H
#include "Coco/ECS/Scene.h"

namespace Coco
{
    /// @brief A system that updates SpritesheetAnimationComponents
    class SpritesheetAnimationComponentSystem
    {
    public:
        /// @brief The order that this system's tick callback will be called
        static constexpr int TickOrder = 50;

        /// @brief Updates the SpritesheetAnimationComponents in the given scene
        /// @param scene The scene being ticked
        /// @param tickInfo The tick info
        static void Tick(Scene& scene, const TickInfo& tickInfo);
    };
} // Coco

#endif //COCOENGINE_SPRITESHEETANIMATIONCOMPONENTSYSTEM_H