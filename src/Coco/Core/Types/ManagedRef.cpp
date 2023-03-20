#include "ManagedRef.h"

namespace Coco
{
	ManagedRefControlBlock::ManagedRefControlBlock() :
		_resourceValid(true)
	{}

	ManagedRefControlBlock::~ManagedRefControlBlock()
	{
		Invalidate();
	}
}
