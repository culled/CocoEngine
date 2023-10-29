#pragma once

#include <Coco/Core/Types/Refs.h>

namespace Coco
{
	/// @brief A simple pool that reuses unused objects
	/// @tparam ObjectType The type of object
	template<typename ObjectType>
	class SharedObjectPool
	{
	private:
		std::vector<SharedRef<ObjectType>> _objects;

	public:
		SharedObjectPool() :
			_objects()
		{}

		~SharedObjectPool()
		{
			_objects.clear();
		}

		/// @brief Gets an unused object from the pool
		/// @return An object with no users
		SharedRef<ObjectType> Get()
		{
			auto it = std::find_if(_objects.begin(), _objects.end(), [](const SharedRef<ObjectType>& o) { return o.use_count() == 1; });

			SharedRef<ObjectType> o;

			if (it != _objects.end())
			{
				o = *it;
			}
			else
			{
				o = CreateSharedRef<ObjectType>();
				_objects.push_back(o);
			}

			return o;
		}
	};
}