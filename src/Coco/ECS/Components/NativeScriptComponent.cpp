//
// Created by cullen on 5/10/26.
//

#include "NativeScriptComponent.h"

#include "Coco/Core/Engine.h"
#include "Coco/ECS/ECSService.h"

namespace Coco
{
    Entity NativeScript::GetOwner() const
    {
        return _scriptComponent->GetOwner();
    }

    NativeScript::NativeScript(NativeScriptComponent* owningComponent) :
        _scriptComponent(owningComponent)
    {}

    DEFINE_RTTI_TYPE(NativeScriptComponent, EntityComponent);

    NativeScriptComponent::NativeScriptComponent(const UUID& ownerID) :
        EntityComponent(ownerID)
    {}

    NativeScriptComponent::~NativeScriptComponent()
    {
        BoundScript.reset();
    }

} // Coco