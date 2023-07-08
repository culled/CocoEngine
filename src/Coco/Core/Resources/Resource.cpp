#include "Resource.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	Resource::Resource(ResourceID id, const string& name) noexcept :
		ID(id), _name(name), _lastTickUsed(0)
	{}

	void Resource::UpdateTickUsed()
	{
		_lastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}
}