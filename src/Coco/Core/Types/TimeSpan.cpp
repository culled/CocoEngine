#include "Corepch.h"
#include "TimeSpan.h"

namespace Coco
{
	TimeSpan::TimeSpan() : 
		Microseconds(0)
	{}

	TimeSpan::TimeSpan(int64 microseconds) :
		Microseconds(microseconds)
	{}
}