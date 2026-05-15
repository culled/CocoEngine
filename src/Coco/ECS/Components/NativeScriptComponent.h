//
// Created by cullen on 5/10/26.
//

#ifndef COCOENGINE_NATIVESCRIPTCOMPONENT_H
#define COCOENGINE_NATIVESCRIPTCOMPONENT_H
#include "Coco/Core/ProcessLoop/TickInfo.h"
#include "Coco/ECS/Entity.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    struct NativeScriptComponent;

    /// @brief Base class for a script that can be bound to a NativeScriptComponent
    class NativeScript
    {
    public:
        virtual ~NativeScript() = default;

        /// @brief Called during the scene tick
        /// @param tickInfo The tick info
        virtual void Tick(const TickInfo& tickInfo) {}

        /// @brief Gets the entity that this script is attached to
        /// @return The entity that this script is attached to
        Entity GetOwner() const;

        /// @brief Creates a component on the entity this script is attached to
        /// @tparam ComponentType The type of component
        /// @tparam Args The constructor arguments
        /// @param args The arguments to pass to the component's constructor
        /// @return The component
        template<typename ComponentType, typename ... Args>
        ComponentType* CreateComponent(Args&& ... args)
        {
            return GetOwner().CreateComponent<ComponentType>(std::forward<Args>(args)...);
        }

        /// @brief Determines if the entity this script is attached to has the given component
        /// @tparam ComponentType The type of component
        /// @return True if the entity has the component
        template<typename ComponentType>
        bool HasComponent() const
        {
            return GetOwner().HasComponent<ComponentType>();
        }

        /// @brief Gets a component on the entity this script is attached to
        /// @tparam ComponentType The type of component
        /// @return The component
        template<typename ComponentType>
        ComponentType* GetComponent()
        {
            return GetOwner().GetComponent<ComponentType>();
        }

        /// @brief Gets a component on the entity this script is attached to
        /// @tparam ComponentType The type of component
        /// @return The component
        template<typename ComponentType>
        const ComponentType* GetComponent() const
        {
            return GetOwner().GetComponent<ComponentType>();
        }

    protected:
        NativeScript(NativeScriptComponent* owningComponent);

    private:
        NativeScriptComponent* _scriptComponent;
    };

    /// @brief An EntityComponent that allows binding a NativeScript
    struct NativeScriptComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(NativeScriptComponent)

    public:
        /// @brief The instance of the bound Native script
        UniquePtr<NativeScript> BoundScript;

        NativeScriptComponent(const UUID& ownerID);
        ~NativeScriptComponent();

        /// @brief Creates an instance of the given script and binds it to this component
        /// @tparam ScriptType The type of script
        /// @tparam Args The constructure argument
        /// @param args The arguments to pass to the script's constructor
        /// @return The bound script
        template<typename ScriptType, typename... Args>
        ScriptType* Bind(Args&& ... args)
        {
            BoundScript = CreateDefaultUnique<ScriptType>(this, std::forward<Args>(args)...);
            return static_cast<ScriptType*>(BoundScript.get());
        }
    };
} // Coco

#endif //COCOENGINE_NATIVESCRIPTCOMPONENT_H