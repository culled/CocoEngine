#pragma once

#include "EntityComponent.h"
#include <Coco/Core/Types/Refs.h>
#include "../Scripting/NativeScript.h"

#include <Coco/Core/Engine.h>

namespace Coco::ECS
{
	/// @brief A component that a NativeScript can be attached to
	class NativeScriptComponent :
		public EntityComponent
	{
		friend class NativeScriptSystem;

	private:
		UniqueRef<NativeScript> _scriptInstance;
		bool _scriptStarted;

	public:
		NativeScriptComponent(const Entity& owner);
		~NativeScriptComponent();

		/// @brief Attaches a NativeScript to this component
		/// @tparam ScriptType The type of script to instantiate
		template<typename ScriptType>
		void Attach()
		{
			static_assert(std::is_base_of<NativeScript, ScriptType>::value, "Script must be derived from NativeScript");

			if (_scriptInstance)
			{
				DestroyAttachedScript();
			}

			_scriptInstance = CreateUniqueRef<ScriptType>();
			_scriptInstance->_entity = GetOwner();

			CocoTrace("Created {} for NativeScriptComponent", typeid(ScriptType).name())

			_scriptInstance->OnCreate();
		}

	private:
		/// @brief Destroys the attached script
		void DestroyAttachedScript();
	};
}