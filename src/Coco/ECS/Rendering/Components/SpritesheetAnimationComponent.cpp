//
// Created by cullen on 4/5/26.
//

#include "SpritesheetAnimationComponent.h"
#include "Coco/Core/ProcessLoop/TickInfo.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(SpritesheetAnimationComponent, EntityComponent);

    SpritesheetAnimationComponent::AnimationData::AnimationData(uint32 startCellIndex, uint32 endCellIndex,
        double framerate) :
        StartCellIndex(startCellIndex),
        EndCellIndex(endCellIndex),
        Framerate(framerate){}

    SpritesheetAnimationComponent::SpritesheetAnimationComponent(const UUID& ownerEntityID) :
        EntityComponent(ownerEntityID),
        CurrentAnimationName(),
        CurrentAnimationTime(0.0f),
        Animations()
    {}

    void SpritesheetAnimationComponent::SetCurrentAnimation(const String& animationName, uint32 startFrame)
    {
        CurrentAnimationName = animationName;
        if (auto anim = Animations.TryGetValue(CurrentAnimationName))
        {
            CurrentAnimationTime = anim->Framerate * startFrame;
        }
    }

    void SpritesheetAnimationComponent::AddAnimation(const String& animationName, uint32 startCellIndex,
        uint32 endCellIndex, double framerate)
    {
        Animations.Emplace(animationName, startCellIndex, endCellIndex, framerate);
    }

    void SpritesheetAnimationComponent::Update(const TickInfo& tickInfo)
    {
        if (!CurrentAnimationName.IsEmpty() && Animations.Contains(CurrentAnimationName))
        {
            CurrentAnimationTime += tickInfo.DeltaTime.GetSeconds();
        }
    }

    uint32 SpritesheetAnimationComponent::GetCurrentAnimationFrame() const
    {
        if (auto anim = Animations.TryGetValue(CurrentAnimationName))
        {
            uint64 frameOffset = std::llroundl(CurrentAnimationTime * anim->Framerate);
            uint64 frameCount = (anim->EndCellIndex - anim->StartCellIndex) + 1;
            return static_cast<uint32>(frameOffset % frameCount) + anim->StartCellIndex;
        }

        return 0;
    }
} // Coco