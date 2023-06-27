#include "Resource.h"
#include <Coco/Core/Engine.h>

namespace Coco
{
	Resource::Resource(ResourceID id, const string& name, uint64_t tickLifetime) noexcept :
		_id(id), _name(name), _tickLifetime(tickLifetime), _lastTickUsed(0)
	{}

	void Resource::UpdateTickUsed()
	{
		_lastTickUsed = Engine::Get()->GetMainLoop()->GetTickCount();
	}

	bool Resource::IsStale() const noexcept
	{
		if (_tickLifetime == MaxLifetime)
			return false;

		return Engine::Get()->GetMainLoop()->GetTickCount() > _lastTickUsed + _tickLifetime;
	}
}