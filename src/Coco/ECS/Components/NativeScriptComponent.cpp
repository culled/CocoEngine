#include "ECSpch.h"
#include "NativeScriptComponent.h"

namespace Coco::ECS
{
	NativeScriptComponent::NativeScriptComponent(const Entity& owner) :
		EntityComponent(owner),
		_scriptInstance(nullptr),
		_scriptStarted(false)
	{}

	NativeScriptComponent::~NativeScriptComponent()
	{
		DestroyAttachedScript();
	}

	void NativeScriptComponent::CreateAttachedScript()
	{
		if (!_createScriptFunc)
			return;

		if (_scriptInstance)
		{
			DestroyAttachedScript();
		}

		_scriptInstance = _createScriptFunc();
		_scriptInstance->_entity = GetOwner();

		_scriptInstance->OnCreate();
	}

	void NativeScriptComponent::DestroyAttachedScript()
	{
		if (!_scriptInstance)
			return;

		_scriptInstance->OnDestroy();
		_scriptInstance.reset();
		_scriptStarted = false;
	}
}