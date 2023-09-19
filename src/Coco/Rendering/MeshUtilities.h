#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Vector.h>
#include "MeshTypes.h"

namespace Coco::Rendering
{
	class MeshUtilities
	{
	public:
		/// @brief Creates a flat grid on the X-Y plane, with default faces facing in the +Z direction
		/// @param size The size of the grid
		/// @param offset The position of the center of the grid
		/// @param format The format of the vertices to add
		/// @param vertices The vertices to add to
		/// @param indices The indices to add to
		/// @param subdivisions The number of cuts along each side of the grid
		/// @param flipDirection If true, the faces & normals will point in the -Z direction
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