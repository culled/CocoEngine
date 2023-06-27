#include "RefControlBlock.h"

namespace Coco
{
	RefControlBlock::RefControlBlock(const std::type_info& type) noexcept : 
		_userCount(1), 
		_resourceType(type)
	{}

	RefControlBlock::RefControlBlock(RefControlBlock&& other) noexcept :
		_resourceType(std::move(other._resourceType))
	{
		_userCount.store(other._userCount);
	}

	RefControlBlock& RefControlBlock::operator=(RefControlBlock&& other) noexcept
	{
		_resourceType = std::move(other._resourceType);
		_userCount.store(other._userCount);

		return *this;
	}
}