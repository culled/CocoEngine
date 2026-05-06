//
// Created by cullen on 4/5/26.
//

#ifndef COCOENGINE_SPRITESHEETANIMATIONCOMPONENT_H
#define COCOENGINE_SPRITESHEETANIMATIONCOMPONENT_H
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    struct TickInfo;

    struct SpritesheetAnimationComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(SpritesheetAnimationComponent)

    public:
        /// @brief Data for a spritesheet animation
        struct AnimationData {
            /// @brief The index of the first cell of the animation
            uint32 StartCellIndex;

            /// @brief The index of the last cell of the animation
            uint32 EndCellIndex;

            /// @brief The framerate that the animation should play at
            double Framerate;

            AnimationData(uint32 startCellIndex, uint32 endCellIndex, double framerate);
        };

        String CurrentAnimationName;
        double CurrentAnimationTime;
        Map<String, AnimationData> Animations;

    public:
        SpritesheetAnimationComponent(const UUID& ownerEntityID);

        void SetCurrentAnimation(const String& animationName, uint32 startFrame = 0);
        void AddAnimation(const String& animationName, uint32 startCellIndex, uint32 endCellIndex, double framerate);
        void Update(const TickInfo& tickInfo);
        uint32 GetCurrentAnimationFrame() const;
    };
} // Coco

#endif //COCOENGINE_SPRITESHEETANIMATIONCOMPONENT_H