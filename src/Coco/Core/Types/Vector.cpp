#include "Corepch.h"
#include "Vector.h"

namespace Coco
{
	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);
	
	Vector2Int::Vector2Int(int x, int y) : Vector2Base<int>(x, y)
	{}
}