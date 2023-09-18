#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Vector.h>
#include "MeshTypes.h"

namespace Coco::Rendering
{
	class MeshUtilities
	{
	public:
		static void CreateXYGrid(
			const Vector2& size,
			const Vector3& offset,
			const VertexDataFormat& format,
			std::vector<VertexData>& vertices,
			std::vector<uint32>& indices,
			uint32 subdivisions = 0,
			bool flipDirection = false);
	};
}