#include "Resource.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	const UUIDv4::UUID Resource::InvalidID = UUIDv4::UUID::UUID("00000000-0000-0000-0000-000000000000");

	Resource::Resource(const ResourceID& id, const string& name) noexcept :
		ID(id), _name(name), _lastTickUsed(0)
	{}

	void Resource::UpdateTickUsed()
	{
		_lastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}
}